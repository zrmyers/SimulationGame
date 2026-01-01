#include "GLTF.hpp"
#include "core/AssetLoader.hpp"
#include "core/Engine.hpp"
#include "core/Logger.hpp"
#include "fastgltf/core.hpp"
#include "fastgltf/tools.hpp"
#include "graphics/pipelines/SkeletalMeshPipeline.hpp"

namespace GLTF {

fastgltf::Asset LoadAsset(Core::Engine& engine, const std::string& filename) {

    Core::AssetLoader& loader = engine.GetAssetLoader();
    fastgltf::Parser parser;

    auto gltfFile =
        fastgltf::MappedGltfFile::FromPath(
            std::filesystem::path(loader.GetMeshDir() + "/" + filename));
    if (gltfFile.error() != fastgltf::Error::None) {
        std::stringstream msg;
        msg << "Failed to open " << filename << ": " << fastgltf::getErrorMessage(gltfFile.error());
        throw Core::EngineException(msg.str());
    }

    auto asset = parser.loadGltfBinary(gltfFile.get(), "");
    if (asset.error() != fastgltf::Error::None) {
        std::stringstream msg;
        msg << "Failed to load gltf " << fastgltf::getErrorMessage(asset.error());
        throw Core::EngineException(msg.str());
    }

    return std::move(asset.get());
}

std::shared_ptr<Graphics::Mesh> LoadSkeletalMesh(Core::Engine& engine, fastgltf::Asset& asset, const std::string& nodeName) {
    try {
        // find the node name in the asset.
        for (fastgltf::Node& node : asset.nodes) {

            if (nodeName.compare(node.name) == 0) { // NOLINT

                if (!node.meshIndex.has_value()) {
                    throw Core::EngineException("No mesh data in " + nodeName);
                }

                fastgltf::Mesh& mesh = asset.meshes.at(node.meshIndex.value());
                fastgltf::Primitive& primitive = mesh.primitives.front(); // assuming single primitive per mesh for now.

                if (mesh.primitives.size() != 1) {
                    throw Core::EngineException("Error, expect all skeletal meshes have only 1 primitive.");
                }

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

                p_mesh->LoadData(vertices, indexBuffer);

                // return mesh.
                return p_mesh;
            }
        }
    } catch (Core::EngineException& error) {

        Core::Logger::Error("Failed to process " + nodeName);
        throw;
    }

    throw Core::EngineException("Failed to find " + nodeName);
}

fastgltf::Accessor& GetAccessor(fastgltf::Asset& asset, fastgltf::Primitive& primitive, const std::string& attributeName) {
    fastgltf::Attribute* p_attribute = primitive.findAttribute(attributeName);
    if (p_attribute == nullptr) {
        throw Core::EngineException("Failed to find attribute: " + attributeName);
    }

    return asset.accessors.at(p_attribute->accessorIndex);
}

}