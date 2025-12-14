#pragma once

#include "core/Logger.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <bitset>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <memory>
#include <sstream>
#include "math/Graph.hpp"

namespace Core {
    class Engine;
}

// ECS implementation based on implementation found here:
// https://austinmorlan.com/posts/entity_component_system/
//
// Tried using a more modern library, however the header files were difficult to follow.
namespace ECS {

    // A simple type alias
    using EntityID_t = uint32_t;

    // Used to define the size of arrays later on
    const EntityID_t MAX_ENTITIES = 10000;

    // A simple type alias
    using ComponentType = std::uint32_t;

    // The maximum number of types of component
    const ComponentType MAX_COMPONENTS = 32;

    // Signature of an entity.
    using Signature_t = std::bitset<MAX_COMPONENTS>;

    // System type code
    using SystemTypeCode_t = size_t;

    // Maximum number of systems
    const SystemTypeCode_t MAX_SYSTEMS = 256;

    //! System Signature
    using SystemDependencies = std::bitset<MAX_SYSTEMS>;

    // Error type
    class Exception : public std::exception {

        public:
            Exception(const std::string& msg)
                : m_msg(msg) {
            }

            const char* what() const override {
                return m_msg.c_str();
            }

        private:
            std::string m_msg;
    };

    class EntityManager {

        public:

            EntityManager() {

                for (EntityID_t entity = 0U; entity < MAX_ENTITIES; entity++) {
                    m_free_entities.push(entity);
                }
            }

            EntityID_t CreateEntity() {
                if (m_free_entities.empty()) {
                    throw ECS::Exception("CreateEntity() failed. No free entity IDs. Consider increasing MAX_ENTITIES.");
                }

                EntityID_t entity = m_free_entities.front();
                m_free_entities.pop();
                m_entity_count++;

                return entity;
            }

            void DestroyEntity(EntityID_t entity) {

                if (entity >= MAX_ENTITIES) {
                    throw ECS::Exception("DestroyEntity() failed. entity ID " + std::to_string(entity) + "is out of range");
                }

                m_signatures.at(entity).reset();

                // recycle ID
                m_free_entities.push(entity);
                m_entity_count--;
            }

            void SetSignature(EntityID_t entity, Signature_t signature) {

                if (entity >= MAX_ENTITIES) {
                    throw ECS::Exception("SetSignature() failed. entity ID " + std::to_string(entity) + "is out of range");
                }

                m_signatures.at(entity) = signature;
            }

            Signature_t& GetSignature(EntityID_t entity) {

                if (entity >= MAX_ENTITIES) {
                    throw ECS::Exception("SetSignature() failed. entity ID " + std::to_string(entity) + "is out of range");
                }

                return m_signatures.at(entity);
            }

        private:

            //! list of free entity IDs.
            std::queue<EntityID_t> m_free_entities;

            //! list of unused entity IDs.
            std::array<Signature_t, MAX_ENTITIES> m_signatures;

            //! Total number of living entities.
            uint32_t m_entity_count{0};
    };

    class IComponentArray {
        public:
            IComponentArray() = default;
            IComponentArray(const IComponentArray& other) = default;
            IComponentArray(IComponentArray&& other) = default;
            IComponentArray& operator=(const IComponentArray& other) = default;
            IComponentArray& operator=(IComponentArray&& other) = default;
            virtual ~IComponentArray() = default;

            virtual void HandleEntityDestroyed(EntityID_t entity) = 0;
    };

    template<typename T>
    class ComponentArray : public IComponentArray {

            //! Initial amount of memory to allocate for components
            static constexpr size_t INITIAL_ARRAY_SIZE = 32;

            //! Maximum amount of memory to allocate for components
            static constexpr size_t MAX_ARRAY_SIZE = MAX_ENTITIES;
        public:

            ComponentArray() : IComponentArray() {
                m_components.reserve(INITIAL_ARRAY_SIZE);
                m_component_to_entity_id.reserve(INITIAL_ARRAY_SIZE);
            }

            void Add(EntityID_t entity, T&& component) {

                if (m_entity_to_component_id.find(entity) != m_entity_to_component_id.end()) {
                    throw Exception("Entity " + std::to_string(entity) + " already has component!");
                }

                if (m_components.capacity() == m_components.size()) {
                    Resize();
                }

                size_t newIndex = m_components.size();
                m_entity_to_component_id[entity] = newIndex;
                m_component_to_entity_id.push_back(entity);
                m_components.push_back(std::move(component));
            }

            T& Emplace(EntityID_t entity) {
                if (m_entity_to_component_id.find(entity) != m_entity_to_component_id.end()) {
                    throw Exception("Entity " + std::to_string(entity) + " already has component!");
                }

                if (m_components.max_size() == m_components.size()) {
                    Resize();
                }

                size_t newIndex = m_components.size();
                m_entity_to_component_id[entity] = newIndex;
                m_component_to_entity_id.push_back(entity);
                return m_components.emplace_back();
            }

            void Remove(EntityID_t entity) {

                auto removeIter = m_entity_to_component_id.find(entity);
                if (removeIter == m_entity_to_component_id.end()) {
                    std::string componentName = std::string(typeid(T).name());
                    throw Exception("Entity " + std::to_string(entity) + " with component " + componentName + " not found.");
                }

                size_t removeIndex = removeIter->second;
                size_t lastIndex = m_components.size() - 1;
                EntityID_t lastEntity = m_component_to_entity_id[lastIndex];

                m_components[removeIndex] = std::move(m_components.at(lastIndex));
                m_components.pop_back();

                m_entity_to_component_id[lastEntity] = removeIndex;
                m_entity_to_component_id.erase(entity);
                m_component_to_entity_id[removeIndex] = lastEntity;
                m_component_to_entity_id.pop_back();
            }

            T& GetByEntity(EntityID_t entity) {

                auto componentIter = m_entity_to_component_id.find(entity);
                if (componentIter == m_entity_to_component_id.end()) {
                    std::string componentName = std::string(typeid(T).name());
                    throw Exception("Entity " + std::to_string(entity) + " with component " + componentName +" not found.");
                }

                return m_components.at(componentIter->second);
            }

            size_t GetSize() const {
                return m_components.size();
            }

            const T& GetByIndex(size_t index) const {
                return m_components.at(index);
            }

            void HandleEntityDestroyed(EntityID_t entity) override {

                if (m_entity_to_component_id.find(entity) != m_entity_to_component_id.end()) {
                    Remove(entity);
                }
            }

        private:

            void Resize() {

                size_t newSize = m_components.size();
                newSize*= 2;
                newSize = std::min(newSize, MAX_ARRAY_SIZE);

                // Increase reservation.
                m_components.reserve(newSize);
                m_component_to_entity_id.reserve(newSize);
            }

            //! should resize as components are added.
            std::vector<T> m_components;

            //! map of component to entity ID.
            std::vector<EntityID_t> m_component_to_entity_id;

            //! map of entity to component IDs.
            std::unordered_map<EntityID_t, size_t> m_entity_to_component_id;
    };

    class ComponentManager {

        public:
            ComponentManager() {
                m_component_arrays.reserve(MAX_COMPONENTS);
            }

            template<typename T>
            void RegisterComponent() {

                const char* p_type_name = typeid(T).name();

                if (m_component_arrays.size() == MAX_COMPONENTS) {
                    throw Exception("Already registered max number of component types. Consider increasing MAX_COMPONENTS.");
                }

                m_component_types[p_type_name] = m_component_arrays.size();
                m_component_arrays.emplace_back(std::make_unique<ComponentArray<T>>());
            }

            template<typename T>
            ComponentType GetComponentType() const
            {
                const char* typeName = typeid(T).name();
                auto typeIter = m_component_types.find(typeName);
                if (typeIter == m_component_types.end()) {
                    throw Exception("Could not find code for type " + std::string(typeName));
                }
                return typeIter->second;
            }

            template<typename T>
            void AddComponent(EntityID_t entity, T&& component) {

                ComponentType typecode = GetComponentType<T>();

                ComponentArray<T>* array = GetComponentArray<T>(typecode);
                array->Add(entity, std::forward(component));
            }

            template<typename T>
            T& EmplaceComponent(EntityID_t entity) {
                ComponentType typecode = GetComponentType<T>();
                ComponentArray<T>* array = GetComponentArray<T>(typecode);
                return array->Emplace(entity);
            }

            template<typename T>
            void RemoveComponent(EntityID_t entity) {

                const char* typeName = typeid(T).name();
                auto typeCodeIter = m_component_types.find(typeName);
                if (typeCodeIter == m_component_types.end()) {

                    throw Exception("Component type not registered for " + std::string(typeName));
                }

                ComponentArray<T>* array = GetComponentArray<T>(typeCodeIter->second);
                array->Remove(entity);
            }

            template<typename T>
            T& GetComponent(EntityID_t entity) {

                const char* typeName = typeid(T).name();
                auto typeCodeIter = m_component_types.find(typeName);
                if (typeCodeIter == m_component_types.end()) {

                    throw Exception("Component type not registered for " + std::string(typeName));
                }

                ComponentArray<T>* array = GetComponentArray<T>(typeCodeIter->second);
                return array->GetByEntity(entity);
            }

            void EntityDestroyed(EntityID_t entity) {
                for (auto& arrayIter : m_component_arrays) {
                    arrayIter->HandleEntityDestroyed(entity);
                }
            }

            template<typename T>
            ComponentArray<T>* GetComponentArray(ComponentType typecode) {
                IComponentArray* p_array = m_component_arrays.at(typecode).get();
                return static_cast<ComponentArray<T>*>(p_array);
            }

            template<typename T>
            const ComponentArray<T>* GetComponentArray(ComponentType typecode) const {
                IComponentArray* p_array = m_component_arrays.at(typecode).get();
                return static_cast<ComponentArray<T>*>(p_array);
            }
        private:

            //! Map from type name to component type integer.
            std::unordered_map<const char*, ComponentType> m_component_types;

            //! Map from type name to a componenet array.
            std::vector<std::unique_ptr<IComponentArray>> m_component_arrays;
    };

    class System {

        public:
            System(Core::Engine& engine) : m_p_engine(&engine) {};
            System(const System& other) = delete;
            System(System&& other) noexcept = default;
            System& operator=(const System& other) = delete;
            System& operator=(System&& other) noexcept = default;
            virtual ~System() = default;

            virtual void Update() = 0;
            virtual void NotifyEntityDestroyed(EntityID_t entityID) {};

            Core::Engine& GetEngine() { return *m_p_engine;};
            std::set<EntityID_t>& GetEntities() { return m_entities;};
            Signature_t& GetSignature() {return m_signature;};

            SystemDependencies& GetDependencies() {return m_dependencies;};

        private:
            Core::Engine* m_p_engine;
            std::set<EntityID_t> m_entities;
            Signature_t m_signature;
            SystemDependencies m_dependencies;
    };

    class SystemManager {

        public:
            SystemManager() {
                m_systems.reserve(MAX_SYSTEMS);
            };

            SystemManager(const SystemManager& other) = delete;
            SystemManager(SystemManager&& other) noexcept = default;
            SystemManager& operator=(const SystemManager& other) = delete;
            SystemManager& operator=(SystemManager&& other) = default;

            ~SystemManager() {

                // Ensure that the last system added is the first one deleted. This
                // will help in case there are dependencies between systems.
                while (!m_systems.empty()) {
                    m_systems.pop_back();
                }
            }

            template<typename T>
            void RegisterSystem(std::unique_ptr<T>&& system) {
                const char* typeName = typeid(T).name();
                if (m_system_index.find(typeName) != m_system_index.end()) {
                    std::stringstream errorMsg;
                    errorMsg << "System, " << typeName << " , already registered.";
                    throw Exception(errorMsg.str());
                }
                if (m_systems.capacity() == m_systems.size()) {
                    throw Exception("Already registered maximum number of systems. Consider increasing MAX_SYSTEMS");
                }

                m_system_index[typeName] = m_systems.size();
                m_systems.push_back(std::move(system));

                m_update_run_order = true;
            }

            template<typename T>
            T& GetSystem() {
                SystemTypeCode_t typecode = GetTypeCode<T>();

                return *dynamic_cast<T*>(m_systems.at(typecode).get());
            }

            template<typename T>
            void SetSignature(const Signature_t& signature) {
                SystemTypeCode_t typecode = GetTypeCode<T>();

                m_systems.at(typecode)->GetSignature() = signature;
            }

            template<typename T>
            const SystemDependencies& GetDependencies() const {
                SystemTypeCode_t typecode = GetTypeCode<T>();

                return m_systems.at(typecode)->GetDependencies();
            }

            template<typename T>
            void SetDependencies(const SystemDependencies& dependencies) {
                SystemTypeCode_t typecode = GetTypeCode<T>();

                m_systems.at(typecode)->GetDependencies() = dependencies;
            }

            void EntityDestroyed(EntityID_t entity) {

                for (auto& systemIter : m_systems) {
                    systemIter->GetEntities().erase(entity);
                }
            }

            void EntitySignatureChanged(EntityID_t entity, const Signature_t& oldSignature, const Signature_t& newSignature) {

                for (auto& systemIter : m_systems) {

                    std::unique_ptr<System>& p_system =systemIter;
                    const Signature_t& systemSignature = p_system->GetSignature();
                    if (systemSignature.any()) {

                        if ((newSignature & systemSignature) == systemSignature) {
                            p_system->GetEntities().insert(entity);
                        }
                        else if ((oldSignature & systemSignature) == systemSignature) {
                            p_system->GetEntities().erase(entity);
                        }
                    }
                }
            }

            void Update() {
                if (m_update_run_order) {

                    Math::Graph<SystemTypeCode_t> systemGraph;
                    std::vector<SystemTypeCode_t> typecodes;
                    std::unordered_map<SystemTypeCode_t, const char*> names;

                    typecodes.reserve(m_system_index.size());
                    // Add nodes to graph
                    for (auto& typecodeIter : m_system_index) {

                        systemGraph.AddNode(typecodeIter.second);
                        typecodes.push_back(typecodeIter.second);
                        names[typecodeIter.second] = typecodeIter.first;
                    }

                    // Add edges to graph
                    for (auto& typecodeIter : m_system_index) {

                        SystemDependencies incoming = m_systems.at(typecodeIter.second)->GetDependencies();

                        for (SystemTypeCode_t dependency : typecodes) {
                            if (incoming.test(dependency)) {

                                systemGraph.AddTransition(dependency, typecodeIter.second);
                            }
                        }
                    }

                    // Run topological sort to get a sorted list of typecodes
                    m_run_order = Math::TopologicalSort(systemGraph);

                    std::stringstream msg;
                    for (SystemTypeCode_t typecode : m_run_order) {
                        msg << "    - " << names[typecode] << "\n";
                    }
                    Core::Logger::Info("System Run Order: \n" + msg.str());
                    m_update_run_order = false;
                }

                for (SystemTypeCode_t typeCode : m_run_order) {

                    // can start adding parallelization here...
                    m_systems.at(typeCode)->Update();
                }
            }

            template<typename T>
            SystemTypeCode_t GetTypeCode() const {
                const char* typeName = typeid(T).name();
                auto typecodeIter = m_system_index.find(typeName);
                if (typecodeIter == m_system_index.end()) {
                    throw Exception("System, " + std::string(typeName) + " not registered.");
                }
                return typecodeIter->second;
            }

        private:

            // System index
            std::unordered_map<const char*, SystemTypeCode_t> m_system_index;

            // Map from system type string to system pointer.
            std::vector<std::unique_ptr<System>> m_systems;

            // Whether the system run order needs to be re-evaluated.
            bool m_update_run_order{true};

            // The order in which systems should be run.
            std::list<SystemTypeCode_t> m_run_order;
    };

    class Registry {

        public:
            Registry() = default;
            Registry(const Registry&) = delete;
            Registry(Registry&& other) noexcept = default;
            Registry& operator=(const Registry&) = delete;
            Registry& operator=(Registry&& other) = default;
            ~Registry() {
                m_systemcallbacks_enabled = false;
            }

            EntityID_t CreateEntity() {
                return m_entity_manager.CreateEntity();
            }

            void DestroyEntity(EntityID_t entity) {
                m_entity_manager.DestroyEntity(entity);
                m_component_manager.EntityDestroyed(entity);
                if (m_systemcallbacks_enabled) {
                    m_system_manager.EntityDestroyed(entity);
                }
            }

            Signature_t GetEntitySignature(EntityID_t entity) {
                return m_entity_manager.GetSignature(entity);
            }

            template<typename T>
            void RegisterComponent() {
                m_component_manager.RegisterComponent<T>();
            }

            template<typename T>
            ComponentType GetComponentType() {
                return m_component_manager.GetComponentType<T>();
            }

            template<typename T>
            Signature_t GetComponentSignature() const {
                ComponentType typecode = m_component_manager.GetComponentType<T>();
                Signature_t signature;
                signature.set(typecode);
                return signature;
            }

            template<typename T>
            void AddComponent(EntityID_t entity, T component) {
                m_component_manager.AddComponent<T>(entity, std::move(component));

                Signature_t& signature = m_entity_manager.GetSignature(entity);
                Signature_t oldSignature = signature;
                signature.set(m_component_manager.GetComponentType<T>());

                m_system_manager.EntitySignatureChanged(entity, oldSignature, signature);
            }

            template<typename T>
            T& EmplaceComponent(EntityID_t entity) {
                T& component = m_component_manager.EmplaceComponent<T>(entity);

                Signature_t& signature = m_entity_manager.GetSignature(entity);
                Signature_t oldSignature = signature;
                signature.set(m_component_manager.GetComponentType<T>());

                m_system_manager.EntitySignatureChanged(entity, oldSignature, signature);

                return component;
            }

            template<typename T>
            T& FindOrEmplaceComponent(EntityID_t entity) {

                ComponentType componentType = m_component_manager.GetComponentType<T>();
                if (m_entity_manager.GetSignature(entity).test(componentType)) {
                    return GetComponent<T>(entity);
                }

                return EmplaceComponent<T>(entity);
            }

            template<typename T>
            bool HasComponent(EntityID_t entity) {

                const Signature_t& componentSignature = GetComponentSignature<T>();
                const Signature_t& entitySignature = GetEntitySignature(entity);
                return (entitySignature & componentSignature).any();
            }

            template<typename T>
            void RemoveComponent(EntityID_t entity) {
                m_component_manager.RemoveComponent<T>(entity);

                Signature_t& signature = m_entity_manager.GetSignature(entity);
                Signature_t oldSignature = signature;
                signature.set(m_component_manager.GetComponentType<T>(), false);

                m_system_manager.EntitySignatureChanged(entity, oldSignature, signature);
            }

            template<typename T>
            T& GetComponent(EntityID_t entity) {
                return m_component_manager.GetComponent<T>(entity);
            }

            template<typename T>
            const ComponentArray<T>* GetComponentArray() const {
                ComponentType typecode = m_component_manager.GetComponentType<T>();
                return m_component_manager.GetComponentArray<T>(typecode);
            }

            template<typename T>
            void RegisterSystem(std::unique_ptr<T>&& p_system) {
                m_system_manager.RegisterSystem(std::move(p_system));
            }

            template<typename T>
            T& GetSystem() {
                return m_system_manager.GetSystem<T>();
            }

            template<typename T>
            void SetSystemSignature(const Signature_t& signature) {
                m_system_manager.SetSignature<T>(signature);
            }

            template<typename Target, typename Dependency>
            void SetSystemDependency() {
                SystemTypeCode_t typecode = m_system_manager.GetTypeCode<Dependency>();
                SystemDependencies dependencies = m_system_manager.GetDependencies<Target>();
                dependencies.set(typecode);
                m_system_manager.SetDependencies<Target>(dependencies);
            }

            void Update() {
                m_system_manager.Update();
            }

        private:

            ComponentManager m_component_manager;
            EntityManager m_entity_manager;
            SystemManager m_system_manager;
            bool m_systemcallbacks_enabled{true};
    };

    class Entity {

        public:
            Entity() : m_id(MAX_ENTITIES), m_p_registry(nullptr) {};
            Entity(ECS::Registry& registry)
                : m_id(registry.CreateEntity())
                , m_p_registry(&registry) {
            }

            Entity(const Entity&) = delete;
            Entity(Entity&& other) noexcept
                : m_id(other.m_id)
                , m_p_registry(other.m_p_registry) {
                other.m_id = MAX_ENTITIES;
                other.m_p_registry = nullptr;
            }
            Entity& operator=(const Entity&) = delete;
            Entity& operator=(Entity&& other) noexcept {
                std::swap(m_id, other.m_id);
                std::swap(m_p_registry, other.m_p_registry);

                return *this;
            }
            ~Entity() {
                if ((m_p_registry != nullptr) && (m_id < MAX_ENTITIES)) {
                    try {
                        m_p_registry->DestroyEntity(m_id);
                    } catch(Exception& error) {
                        Core::Logger::Error(error.what());
                    }
                }
            }

            //! Whether the entity is valid.
            bool IsValid() const {
                return m_id != MAX_ENTITIES;
            }

            template<typename T>
            void AddComponent(T&& component) {
                if (m_p_registry == nullptr) {
                    throw Exception("AddComponent(): entity is not initialized!");
                }
                m_p_registry->AddComponent(m_id, std::forward<T>(component));
            }

            template<typename T>
            T& EmplaceComponent() {
                if (m_p_registry == nullptr) {
                    throw Exception("EmplaceComponent(): entity is not initialized!");
                }
                return m_p_registry->EmplaceComponent<T>(m_id);
            }

            template<typename T>
            bool HasComponent() const {
                if (m_p_registry == nullptr) {
                    throw Exception("HasComponent(): entity is not initialized!");
                }

                return m_p_registry->HasComponent<T>(m_id);
            }

            template<typename T>
            T& GetComponent() {
                if (m_p_registry == nullptr) {
                    throw Exception("GetComponent(): entity is not initialized!");
                }
                return m_p_registry->GetComponent<T>(m_id);
            }

            template<typename T>
            const T& GetComponent() const {
                if (m_p_registry == nullptr) {
                    throw Exception("GetComponent(): entity is not initialized!");
                }
                return m_p_registry->GetComponent<T>(m_id);
            }

            template<typename T>
            T& FindOrEmplaceComponent() {
                if (m_p_registry == nullptr) {
                    throw Exception("FindOrEmplaceComponent(): entity is not initialized!");
                }
                return m_p_registry->FindOrEmplaceComponent<T>(m_id);
            }

            template<typename T>
            void RemoveComponent() {
                if (m_p_registry == nullptr) {
                    throw Exception("RemoveComponent(): entity is not initialized!");
                }
                m_p_registry->RemoveComponent<T>(m_id);
            }

        private:

            EntityID_t m_id;
            ECS::Registry* m_p_registry;
    };
}