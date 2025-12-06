#pragma once

#include "core/Engine.hpp"
#include <string>
#include <utility>

namespace Level {

    // Represents a unique context within the game.
    class Level {

        public:

            //! Constructor for level object
            Level(Core::Engine& engine, std::string name)
                : m_p_engine(&engine)
                , m_name(std::move(name)) {
            }

            //! Don't allow copying
            Level(const Level&) = delete;
            Level& operator=(const Level&) = delete;

            //! Allow moving
            Level(Level&& other) noexcept
                : m_p_engine(other.m_p_engine)
                , m_name(std::move(other.m_name)) {
            }

            Level& operator=(Level&& other) noexcept {
                std::swap(other.m_p_engine, m_p_engine);
                std::swap(other.m_name, m_name);
                return *this;
            }

            virtual ~Level() = default;

            Core::Engine& GetEngine() {
                return *m_p_engine;
            }

            const std::string& GetName() {
                return m_name;
            }

            //! Load resources for the level.
            virtual void Load() = 0;

            //! Update processing for the level.
            virtual void Update() = 0;

            //! Unload resources for the level.
            virtual void Unload() = 0;

        private:

            Core::Engine* m_p_engine;
            std::string m_name;
    };

}