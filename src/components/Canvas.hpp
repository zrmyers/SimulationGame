#pragma once

#include "glm/vec2.hpp"
#include <memory>
#include <vector>
#include "ui/Element.hpp"

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

            //! Given the displaySize, calculate the layout of the canvas and all of its child element.
            //!
            //! This should be called when the canvas is first created, and any time one of the following occurs:
            //! - display size changes
            //! - gui elements are modified.
            void CalculateLayout(glm::vec2 displaySize) {

                CalculateSize(displaySize);
                CalculatePosition(displaySize, {0.0F, 0.0F});

                m_is_dirty = false;
            }

            // Sets the dirty flag. This tells the UI system to recalculate the layout.
            //
            // is_dirty is cleared when the layout is recalculated.
            void SetDirty() {
                m_is_dirty = true;
            }

            // Get the dirty flag.
            bool GetDirty() const {
                return m_is_dirty;
            }

            void UpdateGraphics(ECS::Registry &registry, glm::vec2 screenSize, int depth) override {
                for (auto& child : GetChildren()) {
                    child->UpdateGraphics(registry, screenSize, depth);
                }
            }

        private:
            //! Whether the canvas is in screen or world mode.
            RenderMode m_render_mode {RenderMode::SCREEN};

            //! Whether the canvas layout needs to be recalculated.
            bool m_is_dirty{true};
    };
}