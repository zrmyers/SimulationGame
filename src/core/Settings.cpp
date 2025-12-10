#include "Settings.hpp"

#include "Filesystem.hpp"
#include "core/Filesystem.hpp"
#include "sdl/SDL.hpp"
#include <SDL3/SDL_video.h>
#include <fstream>

#include "nlohmann/json.hpp"

//----------------------------------------------------------------------------------------------------------------------
// GraphicsSettings

Core::GraphicsSettings Core::GraphicsSettings::DetermineDefaults() {

    GraphicsSettings settings;

    SDL_DisplayID primaryDisplay = SDL_GetPrimaryDisplay();
    if (primaryDisplay == 0) {
        throw SDL::Error("SDL_GetPrimaryDisplay() failed!");
    }

    // need to figure out which resolution to use.
    const SDL_DisplayMode* mode = SDL_GetDesktopDisplayMode(primaryDisplay);
    if (mode == nullptr) {
        throw SDL::Error("SDL_GetDisplayMode() failed!");
    }

    settings.SetDisplayResolution({mode->w, mode->h});

    return settings;
}

Core::GraphicsSettings::GraphicsSettings() {
}

void Core::GraphicsSettings::SetDisplayResolution(glm::ivec2 display_resolution) {
    m_display_resolution = display_resolution;
}

glm::ivec2 Core::GraphicsSettings::GetDisplayResolution() const {
    return m_display_resolution;
}

void Core::GraphicsSettings::SetFullscreen(bool fullscreen) {
    m_fullscreen = fullscreen;
}

bool Core::GraphicsSettings::GetFullscreen() const {
    return m_fullscreen;
}

void Core::GraphicsSettings::SetVsyncEnabled(bool vsync) {
    m_vsync_enabled = vsync;
}

bool Core::GraphicsSettings::GetVsyncEnabled() const {
    return m_vsync_enabled;
}

nlohmann::json Core::GraphicsSettings::ToJson() const {
    nlohmann::json graphicsData;

    nlohmann::json displayMode;
    displayMode["x"] = m_display_resolution.x;
    displayMode["y"] = m_display_resolution.y;

    graphicsData["display-mode"] = displayMode;
    graphicsData["vsync"] = m_vsync_enabled;
    graphicsData["fullscreen"] = m_fullscreen;

    return graphicsData;
}

void Core::GraphicsSettings::LoadJson(nlohmann::json& graphicsData) {

    nlohmann::json& displayMode = graphicsData["display-mode"];
    m_display_resolution.x = displayMode["x"];
    m_display_resolution.y = displayMode["y"];

    m_vsync_enabled = graphicsData["vsync"];
    m_fullscreen = graphicsData["fullscreen"];
}

//----------------------------------------------------------------------------------------------------------------------
// Settings

Core::Settings Core::Settings::Load(const std::string& filename) {
    Settings settings;

    if (!Filesystem::FileExists(filename)) {

        settings.GetGraphicsSettings() = GraphicsSettings::DetermineDefaults();

        // save to file
        nlohmann::json settingsData = settings.ToJson();

        std::ofstream outFile(filename);
        outFile << std::setw(4) << settingsData;
    }
    else {

        std::ifstream inFile(filename);

        nlohmann::json settingsData = nlohmann::json::parse(inFile);

        settings.LoadJson(settingsData);
    }

    return settings;
}

Core::Settings::Settings() {
}

Core::GraphicsSettings& Core::Settings::GetGraphicsSettings() {
    return m_graphics;
}

const Core::GraphicsSettings& Core::Settings::GetGraphicsSettings() const {
    return m_graphics;
}

nlohmann::json Core::Settings::ToJson() const {

    nlohmann::json settingsData;
    settingsData["graphics"] = m_graphics.ToJson();

    return settingsData;
}

void Core::Settings::LoadJson(nlohmann::json& settingsData) {

    m_graphics.LoadJson(settingsData["graphics"]);
}