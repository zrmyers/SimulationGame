#pragma once

#include <exception>
#include <unordered_map>
#include <string>
#include <span>
#include "sdl/SDL.hpp"

namespace Core {

    // Generic engine exception.
    class EngineException : public std::exception {

        public:
            EngineException(const std::string& msg);

            const char* what() const override;

        private:
            std::string m_msg;
    };

    //! Class that represents the game engine which runs the simulation.
    class Engine {

        public:

            //! Command line arguments that are passed in to the engine. These are parsed into globally accessible
            //! environment variables that are accessible through the application.
            //!
            //! @param[in] args pass-through from command line.
            Engine(std::span<const char*> args);

            Engine(const Engine& other) = delete;
            Engine(Engine&& other) noexcept = default;
            Engine& operator=(const Engine& other) = delete;
            Engine& operator=(Engine&& other) = default;

            //! Shutdown engine.
            ~Engine();

            //! Read the value of an environment variable in the engine.
            //!
            //! @param[in] name The name of the variable to read.
            //!
            //! A copy of the environment variable.
            std::string ReadEnv(const std::string& name) const;

            //! Main Game loop.
            void Run();

        private:

            //! Initialize the global environment variables.
            void InitEnv(std::span<const char*> args);

            //! Environment variables.
            std::unordered_map<std::string, std::string> m_environment;

            //! SDL context
            SDL::Context m_sdl;

            //! Window used for rendering graphics to screen.
            SDL_Window* m_p_window;

            //! Handle to GPU used for graphics processing
            SDL_GPUDevice* m_p_gpu;
    };

}