#include "CreatureSystem.hpp"
#include "core/AssetLoader.hpp"
#include "core/Logger.hpp"
#include "creature/Compendium.hpp"
#include "ecs/ECS.hpp"
#include <memory>


Systems::CreatureSystem::CreatureSystem(Core::Engine& engine)
    : ECS::System(engine)
    , m_p_skin_pipeline(nullptr)
    , m_p_hair_pipeline(nullptr)
    , m_p_eyes_pipeline(nullptr) {

    Core::AssetLoader& loader = engine.GetAssetLoader();

    m_compendium.Load(engine,loader.GetDataDir() + "/creature.json");
}

void Systems::CreatureSystem::Update() {

    std::set<ECS::EntityID_t>& entities = GetEntities();
    if (!entities.empty()) {

        Core::Logger::Info("Processing Creatures!");
    }
}