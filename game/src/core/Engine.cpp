#include "Engine.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <cstddef>
#include <sstream>
#include <string>
#include "Logger.hpp"

Core::EngineException::EngineException(const std::string& msg)
    : m_msg(msg) {
}

const char* Core::EngineException::what() const {
    return m_msg.c_str();
}

Core::Engine::Engine(std::span<const char*> args) {
    InitEnv(args);

    if (!SDL_Init(SDL_INIT_VIDEO)) {

        throw EngineException("SDL_Init() failed " + std::string(SDL_GetError()));
    }

    // todo: Width, Height should come from display settings
    m_p_window = SDL_CreateWindow("SimulationGame", 1024, 768, SDL_WINDOW_VULKAN);
    if (m_p_window == nullptr) {
        SDL_Quit();
        throw EngineException("SDL_CreateWindow() failed " + std::string(SDL_GetError()));
    }

    if(!SDL_SetWindowPosition(m_p_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED)) {
        Core::Logger::Warning("Failed to center window " + std::string(SDL_GetError()));
    }

    m_p_gpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
    if (m_p_gpu == nullptr) {
        SDL_DestroyWindow(m_p_window);
        SDL_Quit();
        throw EngineException("Failed to initialize GPU " + std::string(SDL_GetError()));
    }

    Core::Logger::Info("OK: Created device with driver: " + std::string(SDL_GetGPUDeviceDriver(m_p_gpu)));

    if (!SDL_ClaimWindowForGPUDevice(m_p_gpu, m_p_window)) {
        SDL_DestroyGPUDevice(m_p_gpu);
        SDL_DestroyWindow(m_p_window);
        SDL_Quit();
        throw EngineException("SDL_ClaimWindowForGPUDevice Failed " + std::string(SDL_GetError()));
    }

}

Core::Engine::~Engine() {
    SDL_ReleaseWindowFromGPUDevice(m_p_gpu, m_p_window);
    SDL_DestroyGPUDevice(m_p_gpu);
    SDL_DestroyWindow(m_p_window);
    SDL_Quit();
}

std::string Core::Engine::ReadEnv(const std::string& name) const {

    std::string value;

    auto valueIter = m_environment.find(name);
    if (valueIter != m_environment.end()) {
        value = valueIter->second;
    }

    return value;
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


void Core::Engine::InitEnv(std::span<const char*> args) {

    // expect that arguments are provided in key=value format.
    for (int argIndex = 1; argIndex < args.size(); argIndex++) {

        std::string argument = std::string(args[argIndex]);

        size_t splitPos = argument.find('=');
        if (splitPos > 0 && splitPos < argument.length() - 1) {
            std::string key = argument.substr(0, splitPos);

            m_environment[key] = argument.substr(splitPos + 1);

            Core::Logger::Info(key + " = " + m_environment[key]);
        }
        else {

            std::stringstream msg;
            msg << "Failed to parse argument from command line, " << argument << "\n"
                << "invalid variable. expect format <key>=<value>" << "\n";

            throw EngineException(msg.str());
        }
    }
}