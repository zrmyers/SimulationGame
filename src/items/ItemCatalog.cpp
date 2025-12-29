#include "ItemCatalog.hpp"
#include "Apparel.hpp"
#include "Materials.hpp"
#include "core/Engine.hpp"
#include "gltf/GLTF.hpp"
#include "json/Json.hpp"

#include <fastgltf/types.hpp>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace Items {

ItemCatalog::ItemCatalog() {
}

//! Loads the catalog from the given filename.
void ItemCatalog::Load(Core::Engine& engine, const std::string& filename) {

    std::ifstream filestream(filename);
    if (!filestream.is_open()) {
        throw Core::EngineException("Could not open " + filename);
    }

    nlohmann::json catalog = nlohmann::json::parse(filestream);

    // load materials
    auto& materialsDataList = catalog["materials"];
    m_material_list.reserve(materialsDataList.size());

    for (auto& materialData : materialsDataList) {

        Material material = {};
        material.m_id = m_material_list.size();
        material.m_name = materialData["id"];
        material.m_description = materialData["description"];
        material.m_color = JSON::ParseColor(materialData["color"]);

        std::string name = material.m_name;
        m_material_list.push_back(std::move(material));
        m_material_map[name] = &m_material_list.back();
    }

    // Load apparel
    auto& apparelDataList = catalog["apparel"];
    m_apparel_list.reserve(apparelDataList.size());

    for (auto& apparelData : apparelDataList) {

        size_t index = m_apparel_list.size();

        Apparel& apparel = m_apparel_list.emplace_back();
        apparel.m_name = apparelData["id"];
        apparel.m_id = index;
        apparel.m_description = apparelData["description"];

        auto& apparelLayers = apparelData["layers"];
        apparel.m_layers.reserve(apparelLayers.size());
        for (std::string layer : apparelLayers) {
            apparel.m_layers.push_back(ParseApparelLayer(layer));
        }

        // coverage is used to update defense related statistics of creature on equip.
        auto& coverageList = apparelData["coverage"];
        apparel.m_coverage.reserve(coverageList.size());
        for (std::string coverage : coverageList) {
            apparel.m_coverage.push_back(std::move(coverage));
        }

        // load apparel variants for loading onto different types of creatures.
        auto& variantList = apparelData["variants"];
        apparel.m_variants.reserve(variantList.size());
        for (auto& variantData : variantList) {

            size_t index = apparel.m_variants.size();
            ApparelVariant& variant = apparel.m_variants.emplace_back();
            variant.m_id = index;
            variant.m_compatible_species = variantData["compatible-species"];
            variant.m_compatible_variant = variantData["compatible-variant"];

            // stuff for loading skeletal mesh.
            fastgltf::Asset asset = GLTF::LoadAsset(engine, variantData["source"]);

            variant.m_p_mesh = std::move(GLTF::LoadSkeletalMesh(engine, asset, variantData["part"]));

        }
        m_apparel_map[apparel.m_name] = &apparel;
    }
}

Material& ItemCatalog::GetMaterialByIndex(MaterialIndex_t index) {
    return m_material_list.at(index);
}

Material& ItemCatalog::GetMaterialByName(const std::string& name) {
    auto materialIter = m_material_map.find(name);

    if (materialIter != m_material_map.end()) {
        return *materialIter->second;
    }

    throw std::invalid_argument("Could not find material with name " + name);
}

//! Get apparel by index or name.
Apparel& ItemCatalog::GetApparelByIndex(ApparelIndex_t index) {

    return m_apparel_list.at(index);
}

Apparel& ItemCatalog::GetApparelByName(const std::string& name) {

    auto apparelIter = m_apparel_map.find(name);

    if (apparelIter != m_apparel_map.end()) {
        return *apparelIter->second;
    }

    throw std::invalid_argument("Could not find apparel with name " + name);
}


ApparelLayers ItemCatalog::ParseApparelLayer(const std::string& layerStr) {

    ApparelLayers layer = ApparelLayers::SKIN;

    if (layerStr == "SKIN") {
        layer = ApparelLayers::SKIN;
    }
    else if (layerStr == "MIDDLE") {
        layer = ApparelLayers::MIDDLE;
    }
    else if (layerStr == "OUTER") {
        layer = ApparelLayers::OUTER;
    }
    else {
        throw Core::EngineException("Could not parse layer string value to ApparelLayers type" + layerStr);
    }

    return layer;
}

}