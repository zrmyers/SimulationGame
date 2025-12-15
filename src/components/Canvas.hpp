#pragma once

#include "glm/vec2.hpp"
#include <memory>
#include <vector>
#include "ui/Element.hpp"

namespace Components {

    //! Used for affecting draw order between multiple convases.
    //!
    //! Canvas Depth sets the most-significant 8 bits for the depth field in the underlying primitive (sprite/text) for
    //! an element. This allows elements from one canvas to be drawn on top of all elements of another canvas if the
    //! first canvas has a higher depth value than the other.
    using CanvasDepth_t = uint8_t;

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
            }

            void UpdateGraphics(ECS::Registry &registry, glm::vec2 screenSize, UI::Depth_t depth) override {

                UI::SetCanvasDepth(depth, m_depth);

                for (auto& child : GetChildren()) {
                    child->UpdateGraphics(registry, screenSize, depth);
                }

                ClearDirty();
            }

            void SetDepth(uint8_t depth) {
                m_depth = depth;
            }

            uint8_t GetDepth() const {
                return m_depth;
            }

        private:
            //! Whether the canvas is in screen or world mode.
            RenderMode m_render_mode {RenderMode::SCREEN};

            //! The canvas depth used for sorting during rendering.
            uint8_t m_depth {0U};
    };
}