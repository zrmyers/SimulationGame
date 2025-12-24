#include "Compendium.hpp"
#include "core/AssetLoader.hpp"
#include "core/Engine.hpp"
#include "core/Logger.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Texture2D.hpp"
#include "sdl/SDL.hpp"
#include "systems/RenderSystem.hpp"
#include <SDL3/SDL_gpu.h>
#include <cstdint>
#include <fastgltf/core.hpp>
#include <fastgltf/math.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>
#include <filesystem>
#include <glm/ext/vector_float2.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <utility>
#include <vector>
#include "graphics/pipelines/SkeletalMeshPipeline.hpp"

namespace Creature {


Material& Species::GetMaterialByName(const std::string& name) {

    for (Material& material : m_materials) {

        if (material.m_name == name) {
            return material;
        }
    }
    throw Core::EngineException("Material not found: " + name);
}

PartType& Species::GetPartTypeByName(const std::string& name) {

    for (PartType& partType : m_part_types) {
        if (partType.m_name == name) {
            return partType;
        }
    }
    throw Core::EngineException("Part Type not found: " + name);
}

void Compendium::Load(Core::Engine& engine, const std::string& filename) {

    std::ifstream filestream(filename);
    if (!filestream.is_open()) {
        throw Core::EngineException("Could not open " + filename);
    }

    nlohmann::json compendium = nlohmann::json::parse(filestream);

    for (auto& capabilityData : compendium["capabilities"]) {

        Capability capability = {};
        capability.m_id = m_capabilities.size();
        capability.m_name = capabilityData["name"];
        capability.m_description = capabilityData["description"];
        m_capabilities.push_back(capability);

        m_capabilities_map[capability.m_name] = &m_capabilities.back();
    }

    if (compendium.contains("species")) {

        for (auto& speciesData : compendium["species"]) {

            try {
                Species species = {};
                species.m_id = m_species.size();
                species.m_name = speciesData["id"];
                species.m_description = speciesData["description"];

                Core::Logger::Info("Loading " + species.m_name);

                // Load Materials
                try {
                    LoadMaterials(engine, species, speciesData);
                } catch (nlohmann::json::exception& error) {
                    Core::Logger::Error("Failed to load materials for species: " + species.m_name);
                    throw;
                }

                // Load Part types
                try {
                    LoadParts(engine, species, speciesData);
                } catch (nlohmann::json::exception& error) {
                    Core::Logger::Error("Failed to load part types for species " + species.m_name);
                    throw;
                }

                // Load Variants
                try {
                    LoadVariants(engine, species, speciesData);
                } catch (nlohmann::json::exception& error) {
                    Core::Logger::Error("Failed to load variant for species " + species.m_name);
                    throw;
                }

                std::string name = species.m_name;
                m_species.push_back(std::move(species));
                m_species_map[name] = &m_species.back();
            } catch (nlohmann::json::exception& error) {

                Core::Logger::Error(error.what());
                throw Core::EngineException("Failed to load species from " + filename);
            }
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

const Capability& Compendium::GetCapabilityByName(const std::string& name) const {

    auto capabilitiesIter = m_capabilities_map.find(name);
    if (capabilitiesIter != m_capabilities_map.end()) {
        return *capabilitiesIter->second;
    }

    throw std::invalid_argument("Could not find capability with name " + name);
}

const Capability& Compendium::GetCapabilityByIndex(CapabilityIndex_t index) const {

    return m_capabilities.at(index);
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

        material.m_colors_count = materialData["inputs"].size();

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

        try {
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

            species.m_variants.push_back(std::move(variant));
        } catch(nlohmann::json::exception& error) {
            Core::Logger::Error("Failed to load variant index " + std::to_string(species.m_variants.size()));
            throw;
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

            if (primitive.type != fastgltf::PrimitiveType::Triangles) {
                throw Core::EngineException("Error, expect all skeletal meshes to be lists of triangles.");
            }

            if (!primitive.indicesAccessor.has_value()) {
                throw Core::EngineException("Error, expect index buffer to be provided from skeletal mesh asset.");
            }

            // Fine required attributes
            fastgltf::Accessor& position = GetAccessor(asset, primitive, "POSITION");
            fastgltf::Accessor& normal = GetAccessor(asset, primitive, "NORMAL");
            fastgltf::Accessor& texcoord = GetAccessor(asset, primitive, "TEXCOORD_0");
            fastgltf::Accessor& joints = GetAccessor(asset, primitive, "JOINTS_0");
            fastgltf::Accessor& weights = GetAccessor(asset, primitive, "WEIGHTS_0");
            fastgltf::Accessor& indices = asset.accessors.at(primitive.indicesAccessor.value());
            std::vector<Graphics::SkeletalMeshVertex> vertices;

            vertices.resize(position.count);

            // construct vertex data from skeletal mesh data in asset
            // POSITION
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset, position,
                [&](fastgltf::math::fvec3 pos, std::size_t index){
                vertices.at(index).position = glm::vec3(pos.x(), pos.y(), pos.z());
            });
            // NORMAL
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset, normal,
                [&](fastgltf::math::fvec3 normal, std::size_t index){
                vertices.at(index).normal = glm::vec3(normal.x(), normal.y(), normal.z());
            });
            // TEXCOORD
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(asset, texcoord,
                [&](fastgltf::math::fvec2 texcoord, std::size_t index){
                vertices.at(index).texcoord = glm::vec2(texcoord.x(), texcoord.y());
            });

            // need to try one of many component type combos
            if ((joints.componentType == fastgltf::ComponentType::UnsignedByte) && (joints.type == fastgltf::AccessorType::Vec4)) {
                // JOINTS
                fastgltf::iterateAccessorWithIndex<fastgltf::math::u8vec4>(asset, joints,
                    [&](fastgltf::math::u8vec4 joints, std::size_t index){
                    vertices.at(index).joints = glm::u8vec4(joints.x(), joints.y(), joints.z(), joints.w());
                });
            }
            else if ((joints.componentType == fastgltf::ComponentType::UnsignedShort) && (joints.type == fastgltf::AccessorType::Vec4)) {
                fastgltf::iterateAccessorWithIndex<fastgltf::math::u16vec4>(asset, joints,
                    [&](fastgltf::math::u16vec4 joints, std::size_t index){
                    // support maximum of 255 bones per skeletal mesh. This means we should verify joint IDs are in bounds.
                    if ((joints.x() > UINT8_MAX) || (joints.y() > UINT8_MAX) || (joints.z() > UINT8_MAX)) {
                        throw Core::EngineException("Error, " + nodeName + " has skeletal mesh with more than maximum supported number of joints.");
                    }
                    vertices.at(index).joints = glm::u8vec4(
                        static_cast<uint8_t>(joints.x()),
                        static_cast<uint8_t>(joints.y()),
                        static_cast<uint8_t>(joints.z()),
                        static_cast<uint8_t>(joints.w())
                    );
                });
            }
            else {
                std::stringstream detailError;
                detailError << "component type: " << std::hex << static_cast<uint32_t>(joints.componentType) << "\n";
                detailError << "accessor type: " << std::hex << static_cast<uint32_t>(joints.type) << "\n";
                throw Core::EngineException("Unsupported component type for skeletal mesh joints.\n" + detailError.str());
            }

            // WEIGHTS
            if ((weights.componentType == fastgltf::ComponentType::Float) && (weights.type == fastgltf::AccessorType::Vec4)) {
                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(asset, weights,
                    [&](fastgltf::math::fvec4 weights, std::size_t index){
                    vertices.at(index).weights = glm::vec4(weights.x(), weights.y(), weights.z(), weights.w());
                });
            }
            else {
                throw Core::EngineException("Unsupported component type for skeletal mesh weights.");
            }

            std::vector<uint16_t> indexBuffer;
            // construct the index data from data in asset
            if(indices.componentType != fastgltf::ComponentType::UnsignedShort) {
                throw Core::EngineException("Error, expect indices to have Unsigned Short Type.");
            }

            indexBuffer.resize(indices.count);
            fastgltf::copyFromAccessor<uint16_t>(asset, indices, indexBuffer.data());

            // create mesh from vertex/index data
            Systems::RenderSystem& renderSystem = engine.GetEcsRegistry().GetSystem<Systems::RenderSystem>();
            std::unique_ptr<Graphics::Mesh> p_mesh = std::make_unique<Graphics::Mesh>(renderSystem.CreateMesh(
                sizeof(Graphics::SkeletalMeshVertex),
                vertices.size(),
                SDL_GPU_INDEXELEMENTSIZE_16BIT,
                indexBuffer.size()));

            // return mesh.
            return p_mesh;
        }
    }

    throw Core::EngineException("Failed to find " + nodeName);
}

fastgltf::Accessor& Compendium::GetAccessor(fastgltf::Asset& asset, fastgltf::Primitive& primitive, const std::string& attributeName) {
    fastgltf::Attribute* p_attribute = primitive.findAttribute(attributeName);
    if (p_attribute == nullptr) {
        throw Core::EngineException("Failed to find attribute: " + attributeName);
    }

    return asset.accessors.at(p_attribute->accessorIndex);
}

}