#include "AssetLoader.hpp"


Core::AssetLoader::AssetLoader(const std::string& content_dir)
    : m_content_dir(content_dir) {
}

std::string Core::AssetLoader::GetShaderDir() {
    return m_content_dir + "/content/shaders";
}

std::string Core::AssetLoader::GetFontDir() {
    return m_content_dir + "/content/fonts";
}

std::string Core::AssetLoader::GetImageDir() {
    return m_content_dir + "/content/images";
}

std::string Core::AssetLoader::GetUiDir() {
    return m_content_dir + "/content/ui";
}

std::string Core::AssetLoader::GetMeshDir() {
    return m_content_dir + "/content/meshes";
}