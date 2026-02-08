#include "PlayerCharacter.hpp"
#include "SimulationGame.hpp"
#include "core/Engine.hpp"
#include <fstream>
#include <list>
#include <string>
#include "core/String.hpp"
#include "creature/Compendium.hpp"
#include "ecs/ECS.hpp"
#include "items/Apparel.hpp"
#include "items/Materials.hpp"
#include "nlohmann/json.hpp"
#include "components/Creature.hpp"
#include "systems/CreatureSystem.hpp"
#include "systems/InventorySystem.hpp"

namespace Character {

static std::string GetCurrentWorldDirectory() {
    Core::Engine& engine = Core::Engine::GetInstance();
    SimulationGame& game = engine.GetGameInstance<SimulationGame>();
    if (game.GetWorld() == nullptr) {
        throw Core::EngineException("No world is currently loaded.");
    }
    std::string worldName = game.GetWorld()->GetParameters().GetName();
    return engine.GetUserSaveDir() + "/worlds/" + worldName + "/";
}

ECS::Entity LoadCharacterFromFile(const std::string& filename) {

    Core::Engine& engine = Core::Engine::GetInstance();
    ECS::Entity characterEntity = ECS::Entity(engine.GetEcsRegistry());
    std::ifstream filestream(GetCurrentWorldDirectory() + "/" + filename);

    nlohmann::json styleData = nlohmann::json::parse(filestream);

    // Deserialize character components here.
    Components::CreatureInstance& creature = characterEntity.EmplaceComponent<Components::CreatureInstance>();
    Systems::InventorySystem& inventorySystem = engine.GetEcsRegistry().GetSystem<Systems::InventorySystem>();
    Systems::CreatureSystem& creatureSystem = engine.GetEcsRegistry().GetSystem<Systems::CreatureSystem>();

    const Creature::Species& species = creatureSystem.GetSpeciesByName(styleData["creature"]["species"]);
    Creature::VariantIndex_t variantID = styleData["creature"]["variant_id"];
    const Creature::Variant& variant = species.m_variants.at(variantID);

    creature.m_p_species = &species;
    creature.m_variant_id = variantID;
    creature.m_name = styleData["creature"]["name"];
    // Load material instances
    for (auto& materialData : styleData["creature"]["materials"]) {
        Creature::MaterialInstance materialInstance = {};
        const Creature::Material& material = species.GetMaterialByName(materialData["name"]);
        materialInstance.m_index = material.m_material_index;
        materialInstance.m_pallete_index = materialData["pallete_index"];
        Creature::MaterialData& data = materialInstance.m_data;
        data.m_type = static_cast<uint32_t>(material.m_shader_type);

        const Creature::ColorPallete& pallete = material.m_pallete.at(materialInstance.m_pallete_index);
        for (uint16_t inputIndex = 0; inputIndex < material.m_colors_count; inputIndex++) {
            data.m_color.at(inputIndex) = *pallete.m_p_colors.at(inputIndex);
        }

        creature.m_material_instance.push_back(materialInstance);
    }
    // Load attachments
    for (auto& attachmentData : styleData["creature"]["attachments"]) {
        Creature::AttachmentInstance attachmentInstance = {};
        const Creature::PartType& partType = species.GetPartTypeByName(attachmentData["part_type"]);
        attachmentInstance.m_part_type_id = partType.m_id;
        attachmentInstance.m_attachment_id = attachmentData["attachment_id"];
        creature.m_attachments.push_back(std::move(attachmentInstance));
    }
    // Load part instances
    for (auto& partData : styleData["creature"]["parts"]) {
        Creature::PartInstance partInstance = {};
        const Creature::Part& part = variant.m_parts.at(partData["part_index"]);
        partInstance.m_part_index = partData["part_index"];
        partInstance.m_part_option_index = partData["part_option_index"];
        partInstance.m_part_health = partData["health"];
        partInstance.m_is_destroyed = partData["is_destroyed"];
        creature.m_part_instances.push_back(std::move(partInstance));
    }
    // Load equipment
    for (auto& apparelData : styleData["creature"]["equipment"]) {

        Items::ApparelInstance apparelInstance = inventorySystem.MakeApparel(apparelData["apparel_id"], apparelData["material_name"]);
        apparelInstance.m_durability = apparelData["durability"];

        creature.m_equipment.push_back(std::move(apparelInstance));
    }
    return characterEntity;
}

void SaveCharacterToFile(const std::string& filename, const ECS::Entity& character) {
    Core::Engine& engine = Core::Engine::GetInstance();
    std::ofstream filestream(GetCurrentWorldDirectory() + "/" + filename);
    Systems::InventorySystem& inventorySystem = engine.GetEcsRegistry().GetSystem<Systems::InventorySystem>();
    nlohmann::json characterData;

    // Serialize character components here.
    const Components::CreatureInstance& creature = character.GetComponent<Components::CreatureInstance>();
    const Creature::Variant& variant = creature.m_p_species->m_variants.at(creature.m_variant_id);

    characterData["creature"]["name"] = creature.m_name;
    characterData["creature"]["species"] = creature.m_p_species->m_name;
    characterData["creature"]["variant_id"] = creature.m_variant_id;
    characterData["creature"]["materials"] = nlohmann::json::array();
    for (const Creature::MaterialInstance& materialInstance : creature.m_material_instance) {
        nlohmann::json materialData;
        const Creature::Material& material = creature.m_p_species->m_materials.at(materialInstance.m_index);
        materialData["name"] = material.m_name;
        materialData["pallete_index"] = materialInstance.m_pallete_index;
        characterData["creature"]["materials"].push_back(materialData);
    }
    characterData["creature"]["attachments"] = nlohmann::json::array();
    for (const Creature::AttachmentInstance& attachment : creature.m_attachments) {
        nlohmann::json attachmentData;
        const Creature::PartType& partType = creature.m_p_species->m_part_types.at(attachment.m_part_type_id);
        attachmentData["part_type"] = partType.m_name;
        attachmentData["attachment_id"] = attachment.m_attachment_id;
        characterData["creature"]["attachments"].push_back(attachmentData);
    }
    // Part Health
    characterData["creature"]["parts"] = nlohmann::json::array();
    for (const Creature::PartInstance& partInstance : creature.m_part_instances) {
        nlohmann::json partData;
        const Creature::Part& part = variant.m_parts.at(partInstance.m_part_index);
        const Creature::PartType& partType = *part.m_p_part_type;
        partData["part_type"] = partType.m_name;
        partData["part_index"] = partInstance.m_part_index;
        partData["part_option_index"] = partInstance.m_part_option_index;
        partData["health"] = partInstance.m_part_health;
        partData["is_destroyed"] = partInstance.m_is_destroyed;
        characterData["creature"]["parts"].push_back(partData);
    }
    characterData["creature"]["equipment"] = nlohmann::json::array();
    for (const Items::ApparelInstance& apparelInstance : creature.m_equipment) {

        nlohmann::json apparelData;
        const Items::Apparel& apparel = inventorySystem.GetItemCatalog().GetApparelByIndex(apparelInstance.m_index);
        apparelData["apparel_id"] = apparel.m_name;
        apparelData["durability"] = apparelInstance.m_durability;

        const Items::Material& material = inventorySystem.GetItemCatalog().GetMaterialByIndex(apparelInstance.m_p_material->m_index);
        apparelData["material_name"] = material.m_name;
        characterData["creature"]["equipment"].push_back(apparelData);
    }
    filestream << characterData.dump(4);
}

std::vector<std::string> GetSavedCharacterFiles() {
    std::vector<std::string> characterFiles;
    Core::Engine& engine = Core::Engine::GetInstance();

    for (const auto& entry : std::filesystem::directory_iterator(GetCurrentWorldDirectory())) {
        if (entry.is_regular_file()) {
            std::string filepath = entry.path().string();
            if (Core::EndsWith(filepath,".player.json")) {
                characterFiles.push_back(entry.path().filename().string());
            }
        }
    }

    return characterFiles;
}


void DeleteCharacterFile(const std::string& filename) {

    Core::Engine& engine = Core::Engine::GetInstance();

    std::string path = engine.GetUserSaveDir() + "/" + filename;
    std::remove(path.c_str());
}

}