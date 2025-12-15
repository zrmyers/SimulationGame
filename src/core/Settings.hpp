#pragma once

#include <cstddef>
#include <glm/ext/vector_int2.hpp>
#include <glm/vec2.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>

namespace Core {

    // Default to 0 means that the render system will probe the display size to use as resolution.
    static constexpr glm::ivec2 DEFAULT_RESOLUTION = {0,0};

    // Default to full screen enabled
    static constexpr bool DEFAULT_FULLSCREEN_ENABLED = true;

    // Default to vsync enabled.
    static constexpr bool DEFAULT_VSYNC_ENABLED = true;

    class GraphicsSettings {

        public:

            static GraphicsSettings DetermineDefaults();

            static std::vector<glm::ivec2> GetSupportedResolutions();
            static std::vector<std::string> ResolutionsToStrings(const std::vector<glm::ivec2>& resolutions);
            static std::string ResolutionToString(glm::ivec2 resolution);
            static size_t FindClosestResolution(const std::vector<glm::ivec2>& supportedResolutions, glm::ivec2 resolution);

            GraphicsSettings() = default;

            void SetDisplayResolution(glm::ivec2 display_resolution);
            glm::ivec2 GetDisplayResolution() const;

            void SetFullscreen(bool fullscreen);
            bool GetFullscreen() const;

            void SetVsyncEnabled(bool vsync);
            bool GetVsyncEnabled() const;

            nlohmann::json ToJson() const;
            void LoadJson(nlohmann::json& graphicsData);

        private:

            glm::ivec2 m_display_resolution {DEFAULT_RESOLUTION};
            bool m_fullscreen {DEFAULT_FULLSCREEN_ENABLED};
            bool m_vsync_enabled {DEFAULT_VSYNC_ENABLED};
    };

    class Settings {

        public:

            static Settings Load(const std::string& filename);

            Settings();

            void SetFilename(std::string filename);

            //! load from filename
            void Load();

            //! save to filename
            void Save();

            GraphicsSettings& GetGraphicsSettings();
            const GraphicsSettings& GetGraphicsSettings() const;

            nlohmann::json ToJson() const;
            void LoadJson(nlohmann::json& settingsData);


        private:

            std::string m_filename;
            GraphicsSettings m_graphics;
    };
}