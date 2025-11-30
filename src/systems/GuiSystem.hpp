#pragma once

#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include <SDL3/SDL_events.h>
#include <string>
namespace Systems {

    class GuiSystem : public ECS::System {

        public:

            GuiSystem(Core::Engine& engine);

            void Update() override;

            //! Set the cursor style for the game.
            void SetCursor(const std::string& image_filename, bool visible);

            //! Whether the cursor should be enabled.
            void SetCursorVisible(bool enable);

            //! Whether the cursor is currently enabled.
            bool GetCursorVisible() const;

        private:

            //! Handle mouse motion
            void HandleMouseMotion(const SDL_MouseMotionEvent& event);

            //! Set the cursor position, using pixel coordinates.
            void UpdateCursor();

            //! The entity used for rendering the cursor.
            ECS::Entity m_cursor_entity;

            //! The latest window size.
            glm::vec2 m_window_size_px;

            //! The latest cursor position.
            glm::vec2 m_cursor_pos_px;

            //! The size of the cursor.
            glm::vec2 m_cursor_size_px;
    };
}