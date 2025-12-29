#include "InventorySystem.hpp"
#include "creature/Compendium.hpp"
#include "ecs/ECS.hpp"
#include "items/Apparel.hpp"
#include "items/Materials.hpp"

namespace Systems {

InventorySystem::InventorySystem(Core::Engine& engine)
    : ECS::System(engine) {

    Core::AssetLoader& loader = engine.GetAssetLoader();
    m_catalog.Load(engine, loader.GetDataDir() + "/items.json");
}

//! The main update function.
void InventorySystem::Update() {

    // do nothing for now.
}

Items::ItemCatalog& InventorySystem::GetItemCatalog() {

    return m_catalog;
}

Items::ApparelInstance InventorySystem::MakeApparel(const std::string& apparel, const std::string& material) {

    Items::ApparelInstance apparelInstance = {};

    Items::Apparel& apparelType = m_catalog.GetApparelByName(apparel);
    Items::Material& materialType = m_catalog.GetMaterialByName(material);

    apparelInstance.m_index = apparelType.m_id;

    Items::MaterialInstance materialInstance = {};
    materialInstance.m_index = materialType.m_id;
    materialInstance.m_data.m_type = static_cast<uint32_t>(Creature::ShaderType::APPAREL);
    materialInstance.m_data.m_color = materialType.m_color;

    apparelInstance.m_p_material = std::make_shared<Items::MaterialInstance>(materialInstance);

    apparelInstance.m_durability = 100U;
    return apparelInstance;
}
}