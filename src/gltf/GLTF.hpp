#pragma once

#include "core/Engine.hpp"
#include "graphics/Mesh.hpp"
#include <memory>
#include <fastgltf/types.hpp>

namespace GLTF {

    fastgltf::Asset LoadAsset(Core::Engine& engine, const std::string& filename);

    std::shared_ptr<Graphics::Mesh> LoadSkeletalMesh(Core::Engine& engine, fastgltf::Asset& asset, const std::string& nodeName);

    fastgltf::Accessor& GetAccessor(fastgltf::Asset& asset, fastgltf::Primitive& primitive, const std::string& attributeName);
}