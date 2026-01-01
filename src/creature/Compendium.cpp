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
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <stdexcept>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <utility>
#include <vector>
#include "json/Json.hpp"
#include "gltf/GLTF.hpp"
#include "core/String.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Creature {

Material& Species::GetMaterialByName(const std::string& name) {

    for (Material& material : m_materials) {

        if (material.m_name == name) {
            return material;
        }
    }
    throw Core::EngineException("Material not found: " + name);
}

const Material& Species::GetMaterialByName(const std::string& name) const {

    for (const Material& material : m_materials) {
        if (material.m_name == name) {
            return material;
        }
    }

    throw Core::EngineException("Material not found: " + name);
}


const Socket& Species::GetSocketByName(const std::string& name) const {

    for (const Socket& socket : m_sockets) {
        if (socket.m_name == name) {
            return socket;
        }
    }

    throw Core::EngineException("Socket not found: " + name);
}

PartType& Species::GetPartTypeByName(const std::string& name) {

    for (PartType& partType : m_part_types) {
        if (partType.m_name == name) {
            return partType;
        }
    }
    throw Core::EngineException("Part Type not found: " + name);
}

const PartType& Species::GetPartTypeByName(const std::string& name) const {

    for (const PartType& partType : m_part_types) {
        if (partType.m_name == name) {
            return partType;
        }
    }
    throw Core::EngineException("Part Type not found: " + name);
}

const Variant& Species::GetMaleVariant() const {

    // Default to first selection if variant not found
    const Variant* p_selection = &m_variants.front();
    for (const Variant& variant : m_variants) {
        if (Core::EndsWith(variant.m_name, "-male")) {
            p_selection = &variant;
        }
    }
    return *p_selection;
}

const Variant& Species::GetFemaleVariant() const {

    // Default to last selection if variant not found
    const Variant* p_selection = &m_variants.back();
    for (const Variant& variant : m_variants) {
        if (Core::EndsWith(variant.m_name, "-female")) {
            p_selection = &variant;
        }
    }
    return *p_selection;
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

                // Load Sockets
                try {
                    LoadSockets(engine, species, speciesData);

                } catch (nlohmann::json::exception& error) {
                    Core::Logger::Error(("Failed to load sockets for species: " + species.m_name));
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

            // parse the color pallete for the material input
            for (auto& color : materialInputData["colors"]) {
                input.m_color_options.push_back(JSON::ParseColor(color));
            }

            uint16_t slot = materialInputData["slot"];
            material.m_colors.at(slot) = input;
        }

        // build material pallete
        for (auto& palleteData : materialData["pallete"]) {

            ColorPallete& pallete = material.m_pallete.emplace_back();
            pallete.m_name = palleteData["name"];
            uint32_t slot_id = 0U;

            for (int32_t index : palleteData["colors"]) {

                MaterialInput& colorInput = material.m_colors.at(slot_id);
                pallete.m_p_colors.push_back(&colorInput.m_color_options.at(index));
                slot_id++;
            }
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

void Compendium::LoadSockets(Core::Engine& engine, Species& species, nlohmann::json& speciesData) {

    species.m_sockets.reserve(speciesData["sockets"].size());

    for (auto& socketData : speciesData["sockets"]) {

        Socket& socket = species.m_sockets.emplace_back();
        socket.m_name = socketData["id"];

        std::string sourceFile = socketData["source-file"];

        fastgltf::Asset asset = GLTF::LoadAsset(engine, sourceFile);

        bool foundNode = false;
        for (fastgltf::Node& node : asset.nodes) {

            if (node.name.compare(socket.m_name) == 0) { // NOLINT

                auto trs = fastgltf::getLocalTransformMatrix(node);

                socket.m_transform = glm::make_mat4x4(trs.data());
                foundNode = true;
            }
        }

        if (!foundNode) {
            throw Core::EngineException("Failed to find socket " + socket.m_name);
        }
    }
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

        if (partData.contains("attachment-point")) {

            // load attachments that are common across all variants.
            partType.m_attachment_point = partData["attachment-point"];

            std::string sourceFile = partData["source-file"];

            fastgltf::Asset asset = GLTF::LoadAsset(engine, sourceFile);

            auto attachmentsList = partData["attachments"];
            partType.m_attachments.reserve(attachmentsList.size());

            for (std::string attachmentName : attachmentsList) {

                Attachment attachment = {};
                attachment.m_id = partType.m_attachments.size();
                attachment.m_name = std::move(attachmentName);
                if (attachment.m_name != "None") {
                    attachment.p_attachments = GLTF::LoadSkeletalMesh(engine, asset, attachment.m_name);
                }
                partType.m_attachments.push_back(std::move(attachment));
            }
        }

        species.m_part_types.push_back(std::move(partType));
    }
}

void Compendium::LoadVariants(Core::Engine& engine, Species& species, nlohmann::json& speciesData) {

    Core::AssetLoader& loader = engine.GetAssetLoader();
    auto variantList = speciesData["variants"];
    species.m_variants.reserve(variantList.size());

    for (auto variantData : variantList) {

        try {
            Variant variant = {};
            variant.m_name = variantData["id"];
            variant.m_description = variantData["description"];
            variant.m_id = species.m_variants.size();

            fastgltf::Asset asset = GLTF::LoadAsset(engine, variantData["source"]);

            // todo load skeleton
            variant.m_skeleton = {};

            // load default apparel
            auto& apparelList = variantData["default-apparel"];
            variant.m_default_apparel.reserve(apparelList.size());
            for (auto& apparelData : apparelList) {
                DefaultApparel& apparel = variant.m_default_apparel.emplace_back();
                apparel.apparel_id = apparelData["apparel-id"];
                apparel.material_id = apparelData["material-id"];
            }

            auto partsList = variantData["parts"];
            variant.m_parts.reserve(partsList.size());

            for (auto partData : partsList) {

                Part part = {};
                part.m_id = variant.m_parts.size();
                part.m_p_part_type = &species.GetPartTypeByName(partData["id"]);

                auto partOptionList = partData["variants"];
                part.m_p_options.reserve(partOptionList.size());

                // load mesh for each part
                for (std::string partVariant : partOptionList) {

                    part.m_p_options.push_back(
                        std::move(GLTF::LoadSkeletalMesh(engine, asset, partVariant)));
                }

                variant.m_parts.push_back(std::move(part));
            }

            species.m_variants.push_back(std::move(variant));
        } catch(nlohmann::json::exception& error) {
            Core::Logger::Error("Failed to load variant index " + std::to_string(species.m_variants.size()));
            throw;
        }
    }
}

}