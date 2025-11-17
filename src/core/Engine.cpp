#include "Engine.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <cstddef>
#include <glslang/Public/ShaderLang.h>
#include <sstream>
#include <string>
#include "Logger.hpp"
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
    , m_window(m_sdl, "Simulation Game", 1024, 768, SDL_WINDOW_VULKAN)
    , m_gpu(m_sdl, SDL_GPU_SHADERFORMAT_SPIRV, true, NULL) {

    m_window.SetPosition( SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    Core::Logger::Info("OK: Created device with driver: " + std::string(m_gpu.GetDriver()));

    m_gpu.ClaimWindow(m_window);

    std::string gamePath = m_env.Get("gamePath");

    // Load file from filesystem
    Graphics::ByteCode spirv = m_shader_cross.CompileToSpirv(
        EShLangVertex, gamePath + "/content/shaders/RawTriangle.vert.hlsl", "main", "/content/shaders", {});

}


Core::Engine::~Engine() {

    m_gpu.ReleaseWindow(m_window);
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
    }
}

