#include "Settings.hpp"
#include <algorithm>
#include <glm/geometric.hpp>
#include <iostream>
#include <sstream>
#include "Filesystem.hpp"
#include "core/Filesystem.hpp"
#include "math/Distance.hpp"
#include "sdl/SDL.hpp"
#include <SDL3/SDL_video.h>
#include <cstdint>
#include <fstream>
#include <set>
#include <vector>

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

std::vector<glm::ivec2> Core::GraphicsSettings::GetSupportedResolutions() {

    // will sort and filter out duplicates
    std::vector<glm::ivec2> supportedResolutions;

    SDL_DisplayID primaryDisplay = SDL_GetPrimaryDisplay();
    if (primaryDisplay == 0) {
        throw SDL::Error("SDL_GetPrimaryDisplay() failed!");
    }

    int32_t mode_count = 0;
    SDL_DisplayMode** pp_modes = SDL_GetFullscreenDisplayModes(primaryDisplay, &mode_count);
    if (pp_modes == nullptr) {
        throw SDL::Error("SDL_GetFullscreenDisplayModes() failed!");
    }

    supportedResolutions.reserve(mode_count);
    for (int32_t mode_index = 0; mode_index < mode_count; mode_index++) {

        SDL_DisplayMode* p_mode = pp_modes[mode_index];
        if (p_mode == nullptr) {
            break;
        }

        // support minimum of 1024x768 resolution.
        if (p_mode->w < 1024) {
            continue;
        }

        supportedResolutions.push_back({p_mode->w, p_mode->h});
    }

    SDL_free(static_cast<void*>(pp_modes));

    std::cout << "num resolutions before de-dup" << supportedResolutions.size();
    std::sort(
        supportedResolutions.begin(),
        supportedResolutions.end(),
        [](const glm::ivec2& left, const glm::ivec2& right){

        return ((left.x + left.y) > (right.x + right.y));
    });

    auto last = std::unique(supportedResolutions.begin(), supportedResolutions.end());
    supportedResolutions.erase(last, supportedResolutions.end());

    std::cout << "num resolutions after de-dup" << supportedResolutions.size();
    return supportedResolutions;
}

std::vector<std::string> Core::GraphicsSettings::ResolutionsToStrings(const std::vector<glm::ivec2>& resolutions) {
    std::vector<std::string> resolutionStrings;
    resolutionStrings.reserve(resolutions.size());

    for (const auto& resolution : resolutions) {
        resolutionStrings.push_back(ResolutionToString(resolution));
    }
    return resolutionStrings;
}

std::string Core::GraphicsSettings::ResolutionToString(glm::ivec2 resolution) {

    std::stringstream toString;
    toString << resolution.x << "x" << resolution.y;

    return toString.str();
}

size_t Core::GraphicsSettings::FindClosestResolution(const std::vector<glm::ivec2>& supportedResolutions, glm::ivec2 resolution) {

    size_t closestIndex = 0U;
    int distance = Math::CalculateManhattenDistance(supportedResolutions.at(0), resolution);

    for (size_t index = 1U; index < supportedResolutions.size(); index++) {

        if (distance == 0) {
            break;
        }

        int newDistance = Math::CalculateManhattenDistance(supportedResolutions.at(index), resolution);
        if (newDistance <= distance) {
            distance = newDistance;
            closestIndex = index;
        }
    }

    return closestIndex;
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

    settings.SetFilename(filename);

    if (!Filesystem::FileExists(filename)) {

        settings.GetGraphicsSettings() = GraphicsSettings::DetermineDefaults();

        settings.Save();
    }
    else {

        settings.Load();
    }

    return settings;
}

Core::Settings::Settings() {
}

void Core::Settings::SetFilename(std::string filename) {
    m_filename = std::move(filename);
}

void Core::Settings::Load() {

    std::ifstream inFile(m_filename);

    nlohmann::json settingsData = nlohmann::json::parse(inFile);

    LoadJson(settingsData);
}

void Core::Settings::Save() {

    // save to file
    nlohmann::json settingsData = ToJson();

    std::ofstream outFile(m_filename);
    outFile << std::setw(4) << settingsData;
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