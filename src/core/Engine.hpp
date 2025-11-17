#pragma once

#include <exception>
#include <string>
#include "Environment.hpp"
#include "sdl/SDL.hpp"
#include "graphics/ShaderCross.hpp"

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
            Engine(const std::list<const char*>& args);
            Engine(const Engine& other) = delete;
            Engine(Engine&& other) = delete;
            Engine& operator=(const Engine& other) = delete;
            Engine& operator=(Engine&& other) = delete;
            ~Engine();

            //! Main Game loop.
            void Run();

        private:

            //! Environment variables.
            Environment m_env;

            //! SDL context
            SDL::Context m_sdl;

            //! Window used for rendering graphics to screen.
            SDL::Window m_window;

            //! Handle to GPU used for graphics processing
            SDL::GpuDevice m_gpu;

            //! Handle Shader computation.
            Graphics::ShaderCross m_shader_cross;

            //! Set of shaders.
            std::list<SDL::Shader> m_shaders;
    };

}