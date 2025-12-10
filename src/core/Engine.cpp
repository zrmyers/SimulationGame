#include "Engine.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <glslang/Public/ShaderLang.h>
#include <string>
#include "Settings.hpp"
#include "sdl/SDL.hpp"

Core::EngineException::EngineException(const std::string& msg)
    : m_msg(msg) {
}

const char* Core::EngineException::what() const {
    return m_msg.c_str();
}

Core::Engine::Engine(const std::list<const char*>& args)
    : m_env(args)
    , m_sdl(SDL_INIT_VIDEO)
    , m_assetLoader(m_env.Get("gamePath"))
    , m_delta_time_sec(0.0F)
    , m_last_time_sec(0.0F)
    , m_keep_running(true) {
    m_sdl.HideCursor();

    std::string preferences = m_sdl.GetPrefPath("Siberian Husky Interactive Games", "Simulation Game");

    m_settings = Settings::Load(preferences + "/settings.json");
}

Core::Engine::~Engine() {
}

Core::AssetLoader& Core::Engine::GetAssetLoader() {
    return m_assetLoader;
}

ECS::Registry& Core::Engine::GetEcsRegistry() {
    return m_registry;
}

Core::Settings& Core::Engine::GetSettings() {
    return m_settings;
}

void Core::Engine::SetGameInstance(std::unique_ptr<IGame>&& p_game) {

    m_game_instance = std::move(p_game);
}

void Core::Engine::Run() {

    while (m_keep_running) {

        m_events.clear();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            switch (event.type) {

                case SDL_EVENT_QUIT:
                    m_keep_running = false;
                    break;

                default:
                    break;
            }

            m_events.push_back(event);
        }

        if (m_game_instance != nullptr) {

            m_game_instance->Update();
        }

        m_registry.Update();

        UpdateDeltaTimeSec();
    }
}

float Core::Engine::GetDeltaTimeSec() const {
    return m_delta_time_sec;
}

const std::vector<SDL_Event>& Core::Engine::GetEvents() {
    return m_events;
}

void Core::Engine::RequestShutdown() {
    m_keep_running = false;
}

void Core::Engine::UpdateDeltaTimeSec() {

    float newTime = static_cast<float>(m_sdl.GetTicks()) / static_cast<float>(SDL_MS_PER_SECOND);
    m_delta_time_sec = newTime - m_last_time_sec;
    m_last_time_sec = newTime;
}