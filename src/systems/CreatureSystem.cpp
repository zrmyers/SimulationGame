#include "CreatureSystem.hpp"
#include "InventorySystem.hpp"
#include "components/Renderable.hpp"
#include "components/Transform.hpp"
#include "core/AssetLoader.hpp"
#include "core/Logger.hpp"
#include "creature/Compendium.hpp"
#include "ecs/ECS.hpp"
#include <cstdint>
#include <memory>
#include "components/Creature.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/pipelines/SkeletalMeshPipeline.hpp"
#include "items/Apparel.hpp"

Systems::CreatureSystem::CreatureSystem(Core::Engine& engine)
    : ECS::System(engine)
    , m_skeletal_mesh_pipeline(nullptr) {

    Core::AssetLoader& loader = engine.GetAssetLoader();

    Systems::RenderSystem& renderSystem = engine.GetEcsRegistry().GetSystem<Systems::RenderSystem>();
    m_compendium.Load(engine,loader.GetDataDir() + "/creature.json");

    m_skeletal_mesh_pipeline = renderSystem.CreatePipeline<Graphics::SkeletalMeshPipeline>();
}

void Systems::CreatureSystem::Update() {

    ECS::Registry& registry = GetEngine().GetEcsRegistry();
    Systems::InventorySystem& inventorySystem = registry.GetSystem<Systems::InventorySystem>();

    std::set<ECS::EntityID_t>& entities = GetEntities();
    if (!entities.empty()) {

        for (const ECS::EntityID_t& entityID : entities) {

            Components::Transform& transform = registry.GetComponent<Components::Transform>(entityID);
            Components::CreatureInstance& creature = registry.GetComponent<Components::CreatureInstance>(entityID);

            const Creature::Variant& variant = creature.m_p_species->m_variants.at(creature.m_variant_id);

            // update renderable for each part
            for (Creature::PartInstance& partInstance : creature.m_part_instances) {

                if (!partInstance.m_entity.IsValid()) {
                    partInstance.m_entity = ECS::Entity(registry);
                }

                // Get the material for a part.
                const Creature::Part& part = variant.m_parts.at(partInstance.m_part_index);
                const Creature::Material& material = *part.m_p_part_type->m_p_material;

                Creature::MaterialInstance& materialInstance = creature.m_material_instance.at(material.m_material_index);

                // Get the mesh for a part.
                const Graphics::Mesh& mesh = *part.m_p_options.at(partInstance.m_part_option_index);

                Components::Renderable& renderable = partInstance.m_entity.FindOrEmplaceComponent<Components::Renderable>();
                renderable.m_p_mesh = &mesh;
                renderable.m_drawcommand.m_num_instances = 1;
                renderable.m_drawcommand.m_start_instance = 0;
                renderable.m_drawcommand.m_start_index = 0;
                renderable.m_drawcommand.m_num_indices = mesh.GetNumIndices();
                renderable.m_drawcommand.m_vertex_offset = mesh.GetVertexOffset();
                if (material.m_p_color_map != nullptr) {
                    renderable.textureSampler = material.m_p_color_map->GetBinding();
                }
                renderable.material.p_data = static_cast<void*>(&materialInstance.m_data);
                renderable.material.data_len = sizeof(materialInstance.m_data);

                renderable.m_layer = Components::RenderLayer::LAYER_3D_OPAQUE;
                renderable.transform = transform.m_transform;
                renderable.m_p_pipeline = m_skeletal_mesh_pipeline;
            }

            for (Creature::AttachmentInstance& attachmentInstance : creature.m_attachments) {

                // get the part type
                const Creature::PartType& partType = creature.m_p_species->m_part_types.at(attachmentInstance.m_part_type_id);

                // Determine if None choise is selected
                const Creature::Attachment& attachment = partType.m_attachments.at(attachmentInstance.m_attachment_id);
                if (attachment.p_attachments == nullptr) {

                    // clear the currently displayed renderable.
                    if (attachmentInstance.m_entity.IsValid()) {
                        attachmentInstance.m_entity = ECS::Entity();
                    }

                    // Cannot render attachment, so skip.
                    continue;
                }

                const Graphics::Mesh& mesh = *attachment.p_attachments;

                // Get the material for a part.
                const Creature::Material& material = *partType.m_p_material;

                Creature::MaterialInstance& materialInstance = creature.m_material_instance.at(material.m_material_index);

                // Get the socket
                const Creature::Socket& socket = creature.m_p_species->GetSocketByName(partType.m_attachment_point);

                if (!attachmentInstance.m_entity.IsValid()) {
                    attachmentInstance.m_entity = ECS::Entity(registry);
                }

                Components::Renderable& renderable = attachmentInstance.m_entity.FindOrEmplaceComponent<Components::Renderable>();
                renderable.m_p_mesh = &mesh;
                renderable.m_drawcommand.m_num_instances = 1;
                renderable.m_drawcommand.m_start_instance = 0;
                renderable.m_drawcommand.m_start_index = 0;
                renderable.m_drawcommand.m_num_indices = mesh.GetNumIndices();
                renderable.m_drawcommand.m_vertex_offset = mesh.GetVertexOffset();
                if (material.m_p_color_map != nullptr) {
                    renderable.textureSampler = material.m_p_color_map->GetBinding();
                }
                renderable.material.p_data = static_cast<void*>(&materialInstance.m_data);
                renderable.material.data_len = sizeof(materialInstance.m_data);

                renderable.m_layer = Components::RenderLayer::LAYER_3D_OPAQUE;
                renderable.transform = transform.m_transform;
                renderable.m_p_pipeline = m_skeletal_mesh_pipeline;
            }

            for (Items::ApparelInstance& apparelInstance : creature.m_equipment) {

                if (!apparelInstance.m_entity.IsValid()) {
                    apparelInstance.m_entity = ECS::Entity(registry);
                }

                Items::Apparel& apparel = inventorySystem.GetItemCatalog().GetApparelByIndex(apparelInstance.m_index);
                Items::ApparelVariant* p_apparelVariant = nullptr;
                // find mesh compatible with creature variant.
                for (Items::ApparelVariant& apparelVariant : apparel.m_variants) {
                    if (apparelVariant.m_compatible_variant == variant.m_name) {
                        p_apparelVariant = &apparelVariant;
                        break;
                    }
                }

                if (p_apparelVariant != nullptr) {
                    Components::Renderable& renderable = apparelInstance.m_entity.FindOrEmplaceComponent<Components::Renderable>();
                    const Graphics::Mesh& mesh = *p_apparelVariant->m_p_mesh;
                    renderable.m_p_mesh = &mesh;
                    renderable.m_drawcommand.m_num_instances = 1;
                    renderable.m_drawcommand.m_start_instance = 0;
                    renderable.m_drawcommand.m_start_index = 0;
                    renderable.m_drawcommand.m_num_indices = mesh.GetNumIndices();
                    renderable.m_drawcommand.m_vertex_offset = mesh.GetVertexOffset();
                    renderable.material.p_data = static_cast<void*>(&apparelInstance.m_p_material->m_data);
                    renderable.material.data_len = sizeof(apparelInstance.m_p_material->m_data);

                    renderable.m_layer = Components::RenderLayer::LAYER_3D_OPAQUE;
                    renderable.transform = transform.m_transform;
                    renderable.m_p_pipeline = m_skeletal_mesh_pipeline;
                }
            }
        }
    }
}

Components::CreatureInstance Systems::CreatureSystem::MakeCreature(const std::string& species_name, Creature::VariantIndex_t variant_id) {

    const Creature::Species& species = m_compendium.GetSpeciesByName(species_name);

    Components::CreatureInstance instance = {};
    instance.m_p_species = &species;

    // setup material instances
    for (const Creature::Material& material : species.m_materials) {

        Creature::MaterialInstance materialInstance = {};
        materialInstance.m_index = material.m_material_index;
        materialInstance.m_pallete_index = 0U;
        Creature::MaterialData& data = materialInstance.m_data;
        data.m_type = static_cast<uint32_t>(material.m_shader_type);

        const Creature::ColorPallete& pallete = material.m_pallete.front();
        for (uint16_t inputIndex = 0; inputIndex < material.m_colors_count; inputIndex++) {
            data.m_color.at(inputIndex) = *pallete.m_p_colors.at(inputIndex);
        }

        // todo will need to have better way of managing materials for creatures at some point.
        instance.m_material_instance.push_back(materialInstance);
    }

    // select a variant.
    const Creature::Variant& variant = species.m_variants.at(variant_id);
    instance.m_variant_id = variant.m_id;

    // create parts from variant
    for (const Creature::Part& part : variant.m_parts) {

        Creature::PartInstance partInstance = {};
        partInstance.m_part_health = 100;
        partInstance.m_part_index = part.m_id;
        partInstance.m_part_option_index = 0;
        partInstance.m_is_destroyed = false;

        // create an entity for the instance. This is used for passing creature parts to other systems.
        partInstance.m_entity = ECS::Entity(GetEngine().GetEcsRegistry());

        instance.m_part_instances.push_back(std::move(partInstance));
    }

    Systems::InventorySystem& inventorySystem = GetEngine().GetEcsRegistry().GetSystem<Systems::InventorySystem>();
    for (const Creature::DefaultApparel& defaultApparel : variant.m_default_apparel) {

        instance.m_equipment.push_back(
            inventorySystem.MakeApparel(defaultApparel.apparel_id, defaultApparel.material_id));
    }

    return instance;
}

Components::CreatureInstance Systems::CreatureSystem::MakeCreature(const std::string& species_name, bool is_male) {

    const Creature::Species& species = m_compendium.GetSpeciesByName(species_name);

    const Creature::Variant& variant = is_male? species.GetMaleVariant() : species.GetFemaleVariant();

    return MakeCreature(species_name, variant.m_id);
}


const Creature::Species& Systems::CreatureSystem::GetSpeciesByName(const std::string& species_name) const {
    return m_compendium.GetSpeciesByName(species_name);
}