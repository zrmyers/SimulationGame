#pragma once

#include <string>

namespace Core {
    class AssetLoader {

        public:

            AssetLoader(const std::string& content_dir);

            std::string GetShaderDir();

            std::string GetFontDir();

        private:

            std::string m_content_dir;
    };
}