#include "CreatureSystem.hpp"
#include "core/AssetLoader.hpp"
#include "core/Logger.hpp"
#include "creature/Compendium.hpp"
#include "ecs/ECS.hpp"
#include <cstdint>
#include <memory>
#include "components/Creature.hpp"

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


Components::CreatureInstance Systems::CreatureSystem::MakeCreature(const std::string& species_name) {

    const Creature::Species& species = m_compendium.GetSpeciesByName(species_name);

    Components::CreatureInstance instance = {};
    instance.m_p_species = &species;

    // setup material instances
    for (const Creature::Material& material : species.m_materials) {

        Creature::MaterialInstance materialInstance = {};
        materialInstance.m_index = material.m_material_index;

        for (uint16_t inputIndex = 0; inputIndex < material.m_colors_count; inputIndex++) {
            materialInstance.m_color_input.at(inputIndex) = material.m_default_colors.at(inputIndex);
        }

        // todo will need to have better way of managing materials for creatures at some point.
        instance.m_material_instance.push_back(std::make_shared<Creature::MaterialInstance>(materialInstance));
    }

    // select a variant.
    const Creature::Variant& variant = species.m_variants.front();

    // create parts from variant
    for (const Creature::Part& part : variant.m_parts) {

        Creature::PartInstance partInstance = {};
        partInstance.m_part_health = 100;
        partInstance.m_part_type_index = part.m_p_part_type->m_id;
        partInstance.m_part_option_index = 0;
        partInstance.m_is_destroyed = false;

        // create an entity for the instance. This is used for passing creature parts to other systems.
        partInstance.m_entity = ECS::Entity(GetEngine().GetEcsRegistry());

        instance.m_part_instances.push_back(std::move(partInstance));
    }

    return instance;
}