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

std::unique_ptr<Core::Engine> Core::Engine::s_instance = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

Core::EngineException::EngineException(const std::string& msg)
    : m_msg(msg) {
}

const char* Core::EngineException::what() const {
    return m_msg.c_str();
}


void Core::Engine::SetInstance(std::unique_ptr<Engine>&& engine) {
    s_instance = std::move(engine);
}

Core::Engine& Core::Engine::GetInstance() {
    if (s_instance == nullptr) {
        throw Core::EngineException("Core::Engine::GetInstance(): Engine instance is not set.");
    }
    return *s_instance;
}

Core::Engine::Engine(const std::list<const char*>& args)
    : m_env(args)
    , m_sdl(SDL_INIT_VIDEO)
    , m_assetLoader(m_env.Get("gamePath"))
    , m_delta_time_sec(0.0F)
    , m_last_time_sec(0.0F)
    , m_keep_running(true) {
    m_sdl.HideCursor();

    m_user_save_dir = m_sdl.GetPrefPath("Siberian Husky Interactive Games", "Simulation Game");

    m_settings = Settings::Load(m_user_save_dir + "/settings.json");
}

Core::AssetLoader& Core::Engine::GetAssetLoader() {
    return m_assetLoader;
}

ECS::Registry& Core::Engine::GetEcsRegistry() {
    return m_registry;
}

const std::string& Core::Engine::GetUserSaveDir() const {
    return m_user_save_dir;
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

float Core::Engine::GetElapsedTimeSec() const {
    return m_last_time_sec;
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