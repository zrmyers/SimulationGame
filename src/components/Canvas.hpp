#pragma once

#include "glm/vec2.hpp"
#include <memory>
#include <vector>
#include "ui/UI.hpp"

namespace Components {

    //! The canvas represents the top-level UI object in a scene.
    //!
    //! The canvas is a container for all other UI objects, performs the following functions:
    //! - Handle User Input (keyboard/mouse) events, routing to the appropriate child object.
    //! - Managing the layout of all child objects
    //! - Transformation of 2D GUI to 3D world or screen space
    class Canvas : public UI::Element {

        public:

            //! The render mode of a Canvas.
            //!
            //! When operating in screen mode, all canvas coordinates are used to calculate layout positions in screen space.
            //!
            //! When operating in world mode, all canvas coordinates are translated to world space.
            enum class RenderMode : uint8_t {
                SCREEN = 0,
                WORLD,
            };

            Canvas& SetRenderMode(RenderMode mode) {
                m_render_mode = mode;
                return *this;
            }

            RenderMode GetRenderMode() const {
                return m_render_mode;
            }

        private:
            //! Whether the canvas is in screen or world mode.
            RenderMode m_render_mode {RenderMode::SCREEN};
    };
}