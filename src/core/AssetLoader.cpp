#include "AssetLoader.hpp"


Core::AssetLoader::AssetLoader(const std::string& content_dir)
    : m_content_dir(content_dir) {
}

std::string Core::AssetLoader::GetShaderDir() {
    return m_content_dir + "/content/shaders";
}