#include "Engine.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
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

Core::Engine::Engine(int argc, const char** argv) {
    InitEnv(argc, argv);

    if (!SDL_Init(SDL_INIT_VIDEO)) {

        throw EngineException("SDL_Init() failed " + std::string(SDL_GetError()));
    }

    m_p_window = SDL_CreateWindow("SimulationGame", 1024, 768, SDL_WINDOW_VULKAN);
    if (m_p_window == nullptr) {
        SDL_Quit();
        throw EngineException("SDL_CreateWindow() failed " + std::string(SDL_GetError()));
    }
}

Core::Engine::~Engine() {

    if (m_p_window != nullptr) {
        SDL_DestroyWindow(m_p_window);
    }
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


void Core::Engine::InitEnv(int argc, const char** argv) {

    // expect that arguments are provided in key=value format.
    for (int argIndex = 1; argIndex < argc; argIndex++) {

        std::string argument = std::string(argv[argIndex]);

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