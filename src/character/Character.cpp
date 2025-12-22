#include "Character.hpp"
#include "core/AssetLoader.hpp"
#include "core/Engine.hpp"
#include "core/Logger.hpp"
#include <fastgltf/core.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace Character {

static Character::PartId NameToMeshID();

std::unique_ptr<Character> Character::CreateDefault(Core::Engine& engine) {

    Core::AssetLoader& loader = engine.GetAssetLoader();
    std::unique_ptr<Character> p_character = std::make_unique<Character>();

    fastgltf::Parser parser;
    std::filesystem::path assetPath(loader.GetMeshDir() + "/Human_Male.glb");

    // Load meshes & setup materials
    auto gltfFile = fastgltf::MappedGltfFile::FromPath(assetPath);
    if (!bool(gltfFile)) {
        throw Core::EngineException("MappedGltfFile::fromPath() failed! " + std::string(fastgltf::getErrorMessage(gltfFile.error())));
    }

    auto asset = parser.loadGltf(gltfFile.get(), "");
    if (asset.error() != fastgltf::Error::None) {
        throw Core::EngineException("loadGltf() failed! " + std::string(fastgltf::getErrorMessage(gltfFile.error())));
    }

    for (const fastgltf::Node& node : asset->nodes) {

        Core::Logger::Info("GLTF Node: " + std::string(node.name));
    }

    p_character->m_sex = Character::Sex::MALE;

    return p_character;
}

}