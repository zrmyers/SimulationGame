#pragma once

#include <SDL3/SDL_events.h>
#include <exception>
#include <memory>
#include <string>
#include <vector>
#include "AssetLoader.hpp"
#include "Environment.hpp"
#include "IGame.hpp"
#include "sdl/SDL.hpp"
#include "ecs/ECS.hpp"

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

            //! Get the Asset loader instance
            AssetLoader& GetAssetLoader();

            //! Get the ECS registry
            ECS::Registry& GetEcsRegistry();

            //! Set the Game Instance.
            void SetGameInstance(std::unique_ptr<IGame>&& game);

            //! Main Game loop.
            void Run();

            //! Get the latest delta time.
            float GetDeltaTimeSec() const;

            //! Get latest input events
            const std::vector<SDL_Event>& GetEvents();

            void RequestShutdown();

        private:

            //! update the delta time.
            void UpdateDeltaTimeSec();

            //! Environment variables.
            Environment m_env;

            //! SDL context
            SDL::Context m_sdl;

            //! Asset loading
            AssetLoader m_assetLoader;

            //! ECS registry.
            ECS::Registry m_registry;

            //! Game instance
            std::unique_ptr<IGame> m_game_instance;

            //! Latest events
            std::vector<SDL_Event> m_events;

            //! The latest delta frame time, in seconds.
            float m_delta_time_sec;

            //! The last frame time, in seconds.
            float m_last_time_sec;

            //! Whether the engine should keep running
            bool m_keep_running;
    };

}