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
    , m_last_time_sec(0.0F) {
}

Core::Engine::~Engine() {
}

Core::AssetLoader& Core::Engine::GetAssetLoader() {
    return m_assetLoader;
}

ECS::Registry& Core::Engine::GetEcsRegistry() {
    return m_registry;
}

void Core::Engine::SetGameInstance(std::unique_ptr<IGame>&& p_game) {

    m_game_instance = std::move(p_game);
}

void Core::Engine::Run() {

    bool shouldQuit = false;

    while (!shouldQuit) {

        SDL_Event event;
        if (SDL_PollEvent(&event)) {

            switch (event.type) {

                case SDL_EVENT_QUIT:
                    shouldQuit = true;
                    break;

                default:
                    break;
            }
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

void Core::Engine::UpdateDeltaTimeSec() {

    float newTime = static_cast<float>(m_sdl.GetTicks()) / static_cast<float>(SDL_MS_PER_SECOND);
    m_delta_time_sec = newTime - m_last_time_sec;
    m_last_time_sec = newTime;
}