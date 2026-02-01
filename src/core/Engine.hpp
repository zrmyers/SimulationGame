#pragma once

#include <SDL3/SDL_events.h>
#include <exception>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "AssetLoader.hpp"
#include "Environment.hpp"
#include "IGame.hpp"
#include "NameGenerator.hpp"
#include "Settings.hpp"
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

            //! Set the engine instance.
            static void SetInstance(std::unique_ptr<Engine>&& engine);

            //! Get the engine instance.
            static Engine& GetInstance();

            //! Command line arguments that are passed in to the engine. These are parsed into globally accessible
            //! environment variables that are accessible through the application.
            //!
            //! @param[in] args pass-through from command line.
            Engine(const std::list<const char*>& args);
            Engine(const Engine& other) = delete;
            Engine(Engine&& other) noexcept = default;
            Engine& operator=(const Engine& other) = delete;
            Engine& operator=(Engine&& other) noexcept = default;
            ~Engine() = default;

            //! Get the Asset loader instance
            AssetLoader& GetAssetLoader();

            //! Get the ECS registry
            ECS::Registry& GetEcsRegistry();

            //! Get the user save directory
            const std::string& GetUserSaveDir() const;

            //! Get the settings
            Settings& GetSettings();

            //! Set the Game Instance.
            void SetGameInstance(std::unique_ptr<IGame>&& game);

            //! Get the game instance
            template <typename T>
            T& GetGameInstance() {
                if (m_game_instance == nullptr) {
                    throw EngineException("GetGameInstance(): GameInstance not initialized!");
                }
                return *dynamic_cast<T*>(m_game_instance.get());
            }

            //! Main Game loop.
            void Run();

            //! Get the latest delta time.
            float GetDeltaTimeSec() const;

            //! Get the time since the game started, in seconds.
            float GetElapsedTimeSec() const;

            //! Get latest input events
            const std::vector<SDL_Event>& GetEvents();

            void RequestShutdown();

            //! Add a name generator to the engine
            void AddNameGenerator(const std::string& name_type, const std::string& name_file);

            //! Get a name generator.
            NameGenerator& GetNameGenerator(const std::string& name_type);
        private:

            //! update the delta time.
            void UpdateDeltaTimeSec();

            //! The engine instance.
            static std::unique_ptr<Engine> s_instance;

            //! Environment variables.
            Environment m_env;

            //! SDL context
            SDL::Context m_sdl;

            //! Asset loading
            AssetLoader m_assetLoader;

            //! ECS registry.
            ECS::Registry m_registry;

            //! User Save Directory
            std::string m_user_save_dir;

            //! Game Settings
            Settings m_settings;

            //! Game instance
            std::unique_ptr<IGame> m_game_instance;

            //! Latest events
            std::vector<SDL_Event> m_events;

            //! Name generator
            std::unordered_map<std::string, std::unique_ptr<NameGenerator>> m_name_generator;

            //! The latest delta frame time, in seconds.
            float m_delta_time_sec;

            //! The last frame time, in seconds.
            float m_last_time_sec;

            //! Whether the engine should keep running
            bool m_keep_running;
    };

}