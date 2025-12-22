#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Core {

    using AssetType_t = uint32_t;

    class Asset {

        public:
            Asset() = default;
    };

    class Stream {

        public:
            Stream(const std::string& filename);

            const std::string& GetFilename();

        private:
            std::string m_filename;
    };

    class ITypeLoader {

        public:
            ITypeLoader() = default;
            ITypeLoader(const ITypeLoader&) = delete;
            ITypeLoader(ITypeLoader&&) = default;
            ITypeLoader& operator=(const ITypeLoader&) = delete;
            ITypeLoader& operator=(ITypeLoader&&) = delete;
            virtual ~ITypeLoader() = default;

            virtual std::shared_ptr<Asset> Load(Stream& stream) = 0;
    };

    class AssetLoader {

        public:

            AssetLoader(const std::string& content_dir);

            std::string GetShaderDir();

            std::string GetFontDir();

            std::string GetImageDir();

            std::string GetUiDir();

            std::string GetMeshDir();

            template<typename T>
            std::shared_ptr<T> Load(const std::string& name) {

                Stream stream(name);
            }

        private:

            template<typename T>
            AssetType_t GetTypeCode() {
                const char* typeName = typeid(T).name();

            }

            // lookup asset type code
            std::unordered_map<const char*, AssetType_t> m_asset_types;

            // Use type code to index into directory list.
            std::vector<std::string> m_directories;

            // lookup loader by type
            std::vector<std::unique_ptr<ITypeLoader>> m_loaders;

            // content directory
            std::string m_content_dir;

    };
}