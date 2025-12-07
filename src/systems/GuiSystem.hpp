#pragma once

#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "components/Canvas.hpp"
#include <SDL3/SDL_events.h>
#include <string>
#include <vector>
namespace Systems {

    class GuiSystem : public ECS::System {

        public:

            GuiSystem(Core::Engine& engine);

            void Update() override;

            void NotifyEntityDestroyed(ECS::EntityID_t entityID) override;

            //! Set the cursor style for the game.
            void SetCursor(const std::string& image_filename, bool visible);

            //! Whether the cursor should be enabled.
            void SetCursorVisible(bool enable);

            //! Whether the cursor is currently enabled.
            bool GetCursorVisible() const;

        private:

            //! Handle mouse motion
            void HandleMouseMotion(const SDL_MouseMotionEvent& event);

            //! Handle resize
            void HandleWindowResize();

            //! Set the cursor position, using pixel coordinates.
            void UpdateCursor();

            //! Process Canvas
            void ProcessCanvas(Components::Canvas& canvas, const std::vector<SDL_Event>& events);

            //! The entity used for rendering the cursor.
            ECS::Entity m_cursor_entity;

            //! The latest window size.
            glm::vec2 m_window_size_px;

            //! The latest cursor position.
            glm::vec2 m_cursor_pos_px;

            //! The previous cursor position.
            glm::vec2 m_prev_cursor_pos_px;

            //! The size of the cursor.
            glm::vec2 m_cursor_size_px;

    };
}