#include "Compendium.hpp"
#include "core/AssetLoader.hpp"
#include "core/Engine.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Texture2D.hpp"
#include "sdl/SDL.hpp"
#include "systems/RenderSystem.hpp"
#include <SDL3/SDL_gpu.h>
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <filesystem>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Creature {

void Compendium::Load(Core::Engine& engine, const std::string& filename) {

    Core::AssetLoader& assetLoader = engine.GetAssetLoader();

    std::ifstream filestream(assetLoader.GetUiDir() + "/" + filename);

    nlohmann::json compendium = nlohmann::json::parse(filestream);

    for (auto& capabilityData : compendium["capabilities"]) {

        Capability capability = {};
        capability.m_id = m_capabilities.size();
        capability.m_name = capabilityData["name"];
        capability.m_description = capabilityData["description"];
    }

    if (compendium.contains("species")) {

        for (auto& speciesData : compendium["species"]) {

            Species species = {};
            species.m_id = m_species.size();
            species.m_name = speciesData["id"];
            species.m_description = speciesData["description"];

            // Load Materials
            LoadMaterials(engine, species, speciesData);

            // Load Part types
            LoadParts(engine, species, speciesData);

            // Load Variants
            LoadVariants(engine, species, speciesData);
        }
    }
}

const Species& Compendium::GetSpeciesByIndex(SpeciesIndex_t index) {

    if (index > m_species.size()) {
        throw std::invalid_argument("index exceeds species list size. " + std::to_string(index));
    }

    return m_species.at(index);
}

const Species& Compendium::GetSpeciesByName(const std::string& name) {

    auto speciesIter = m_species_map.find(name);
    if (speciesIter != m_species_map.end()) {
        return *speciesIter->second;
    }

    throw std::invalid_argument("Could not find species with name " + name);
}


void Compendium::LoadMaterials(Core::Engine& engine, Species& species, nlohmann::json& speciesData) {

    Core::AssetLoader& loader = engine.GetAssetLoader();

    species.m_materials.reserve(speciesData["materials"].size());

    for (auto& materialData : speciesData["materials"]) {

        Material material = {};
        material.m_material_index = species.m_materials.size();
        material.m_name = materialData["id"];
        material.m_description = materialData["description"];

        if (materialData.contains("color-map")) {

            // Load image to RAM
            std::string image_name = materialData["color-map"];
            SDL::Image image(loader.GetImageDir() + "/" + image_name);

            // Create Texture
            material.m_p_color_map = std::make_unique<Graphics::Texture2D>(
                engine,
                GetTextureSampler(engine),
                image.GetWidth(),
                image.GetHeight(),
                true);

            // Load image data.
            material.m_p_color_map->LoadImageData(image);
        }

        std::string shaderTypeStr = materialData["shader-type"];
        if (shaderTypeStr == "skin") {
            material.m_shader_type = ShaderType::SKIN;
        }
        else if (shaderTypeStr == "eyes") {
            material.m_shader_type = ShaderType::EYES;
        }
        else {
            material.m_shader_type = ShaderType::HAIR;
        }

        // process material inputs
        for (auto& materialInputData : materialData["inputs"]) {

            MaterialInput input = {};
            input.m_name = materialInputData["name"];
            input.m_description = materialInputData["description"];

            uint16_t slot = materialInputData["slot"];
            material.m_colors.at(slot) = input;

            material.m_default_colors.at(slot) = ParseColor(materialInputData["default"]);
        }

        species.m_materials.push_back(std::move(material));
    }
}

std::shared_ptr<SDL::GpuSampler> Compendium::GetTextureSampler(Core::Engine& engine) {
    if (m_p_sampler == nullptr) {

        Systems::RenderSystem& renderSystem = engine.GetEcsRegistry().GetSystem<Systems::RenderSystem>();

        SDL_GPUSamplerCreateInfo createInfo = {};
        createInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        createInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        createInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        createInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
        createInfo.enable_anisotropy = true;
        createInfo.max_anisotropy = 16;
        createInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
        createInfo.min_filter = SDL_GPU_FILTER_LINEAR;

        m_p_sampler = std::make_shared<SDL::GpuSampler>(renderSystem.CreateSampler(createInfo));
    }
    return m_p_sampler;
}

glm::vec4 Compendium::ParseColor(nlohmann::json& colorData) {

    glm::vec4 color(1.0F);

    color.r = colorData["r"];
    color.g = colorData["g"];
    color.b = colorData["b"];
    color.a = colorData["a"];

    return color;
}

void Compendium::LoadParts(Core::Engine& engine, Species& species, nlohmann::json& speciesData) const {

    auto& partsListData = speciesData["parts"];
    species.m_part_types.reserve(partsListData.size());
    for (auto& partData : partsListData) {

        PartType partType = {};
        partType.m_name = partData["id"];
        partType.m_description = partData["description"];
        partType.m_id = species.m_part_types.size();
        partType.m_p_material = &species.GetMaterialByName(partData["material-id"]);

        for (std::string capabilityName : partData["provides"]) {

            partType.m_p_provides.push_back(&GetCapabilityByName(capabilityName));
        }

        species.m_part_types.push_back(std::move(partType));
    }
}

void Compendium::LoadVariants(Core::Engine& engine, Species& species, nlohmann::json& speciesData) const {

    Core::AssetLoader& loader = engine.GetAssetLoader();
    auto variantList = speciesData["variants"];
    species.m_variants.reserve(variantList.size());

    for (auto variantData : variantList) {

        Variant variant = {};
        variant.m_name = variantData["id"];
        variant.m_description = variantData["description"];
        variant.m_id = species.m_variants.size();

        fastgltf::Parser parser;

        std::string meshSourceFile = variantData["source"];
        auto gltfFile =
            fastgltf::MappedGltfFile::FromPath(
                std::filesystem::path(loader.GetMeshDir() + "/" + meshSourceFile));
        if (gltfFile.error() != fastgltf::Error::None) {
            std::stringstream msg;
            msg << "Failed to open " << meshSourceFile << ": " << fastgltf::getErrorMessage(gltfFile.error());
            throw Core::EngineException(msg.str());
        }

        auto asset = parser.loadGltfBinary(gltfFile.get(), "");
        if (asset.error() != fastgltf::Error::None) {
            std::stringstream msg;
            msg << "Failed to load gltf " << fastgltf::getErrorMessage(asset.error());
            throw Core::EngineException(msg.str());
        }

        // todo load skeleton
        variant.m_skeleton = {};

        auto partsList = variantData["parts"];
        variant.m_parts.reserve(partsList.size());

        for (auto partData : partsList) {

            Part part = {};
            part.m_p_part_type = &species.GetPartTypeByName(partData["id"]);

            auto partOptionList = partData["variants"];
            part.m_p_options.reserve(partOptionList.size());

            // load mesh for each part
            for (std::string partVariant : partOptionList) {

                part.m_p_options.push_back(
                    std::move(LoadSkeletalMesh(engine, asset.get(), partVariant)));
            }
        }
    }
}


std::unique_ptr<Graphics::Mesh> Compendium::LoadSkeletalMesh(Core::Engine& engine, fastgltf::Asset& asset, const std::string& nodeName) {

    // find the node name in the asset.
    for (fastgltf::Node& node : asset.nodes) {

        if (nodeName.compare(node.name) == 0) { // NOLINT

            if (!node.meshIndex.has_value()) {
                throw Core::EngineException("No mesh data in " + nodeName);
            }

            fastgltf::Mesh& mesh = asset.meshes.at(node.meshIndex.value());
            fastgltf::Primitive& primitive = mesh.primitives.front(); // assuming single primitive per mesh for now.


            // construct vertex data from skeletal mesh data in asset

            // create mesh from vertex/index data

            // return mesh.

        }
    }

    throw Core::EngineException("Failed to find " + nodeName);
}

}