#pragma once

#include "glm/vec2.hpp"

#include <functional>
#include <memory>
#include <cstdint>

namespace ECS {
    class Registry;
}

// defines the base elements from which a UI can be built.
namespace UI {

    enum class MouseButtonID : uint8_t {
        MOUSE_LEFT = 0,
        MOUSE_RIGHT
    };

    enum class LayoutMode : uint8_t {
        RELATIVE_TO_PARENT = 0,
        FIT_TO_CHILDREN,
        FIXED
    };

    using HoverCallback_t = std::function<void(void)>;
    using MouseButtonCallback_t = std::function<void(MouseButtonID)>;

    //! Element.
    class Element {

        public:
            Element() = default;
            Element(const Element& other) = delete;
            Element(Element&& other) = default;
            Element& operator=(const Element& other) = delete;
            Element& operator=(Element&& other) = default;
            virtual ~Element() = default;

            Element& SetOrigin(glm::vec2 origin);
            Element& SetRelativeSize(glm::vec2 relative_size);
            Element& SetRelativePosition(glm::vec2 relative_position);
            Element& SetOffsetSize(glm::vec2 offset_size);
            Element& SetOffsetPosition(glm::vec2 offset_position);
            Element& SetFixedSize(glm::vec2 fixed_size);
            Element& SetLayoutMode(LayoutMode mode);

            glm::vec2 GetOrigin() const;
            glm::vec2 GetRelativeSize() const;
            glm::vec2 GetRelativePosition() const;
            glm::vec2 GetOffsetSize() const;
            glm::vec2 GetOffsetPosition() const;
            glm::vec2 GetFixedSize() const;
            LayoutMode GetLayoutMode() const;

            //! Calculate the size of the element.
            //!
            //! @param[in] parent_size The absolute size of the parent element.
            virtual void CalculateSize(glm::vec2 parent_size);

            //! Calculate the position of the element
            //!
            //! @param[in] parent_size The absolute size of the parent element.
            //! @param[in] parent_position The absolute position of the parent element.
            virtual void CalculatePosition(glm::vec2 parent_size, glm::vec2 parent_position);

            //! Process mouse movement
            void OnHover(glm::vec2 prev_position_px, glm::vec2 current_position_px);

            //! Process mouse click
            void OnMousePress(glm::vec2 press_position, MouseButtonID button_id);

            //! Process mouse release
            void OnMouseRelease(glm::vec2 release_position, MouseButtonID button_id);

            //! Update the element. If element is visible, updates sprite and text primitives.
            virtual void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) = 0;

            glm::vec2 GetAbsoluteSize() const;
            glm::vec2 GetAbsolutePosition() const;

            void AddChild(std::unique_ptr<Element>&& child);

            template<typename T>
            T& EmplaceChild() {
                std::unique_ptr<Element>& child = m_children.emplace_back(std::make_unique<T>());
                return *(static_cast<T*>(child.get()));
            }

            std::vector<std::unique_ptr<Element>>& GetChildren();
            size_t GetChildCount();
            Element& GetChild(size_t child_index);

            bool CheckCollision(glm::vec2 point_px) const;

            Element& SetHoverEnterCallback(HoverCallback_t callback);
            Element& SetHoverExitCallback(HoverCallback_t callback);
            Element& SetMouseButtonPressCallback(MouseButtonCallback_t callback);
            Element& SetMouseButtonReleaseCallback(MouseButtonCallback_t callback);

        protected:

            void SetAbsoluteSize(glm::vec2 size);
            void SetAbsolutePosition(glm::vec2 position);

        private:

            // The current layout mode of the element.
            LayoutMode m_layout_mode{LayoutMode::RELATIVE_TO_PARENT};

            // Relative size to the parent.
            //
            // (1.0, 1.0) - element has same size as parent
            // (0.5, 0.5) - element is half the size of parent.
            glm::vec2 m_relative_size{1.0F, 1.0F};

            // Relative position to parent.
            //
            // (0.5, 0.5) - element is centered on parent.
            // (1.0, 1.0) - element is located at bottom right corner of parent.
            glm::vec2 m_relative_position {0.0F, 0.0F};

            // Offset in pixels to add to the size during calculation.
            glm::vec2 m_offset_size {0.0F, 0.0F};

            // Offset in pixels to add to the position during calculation.
            glm::vec2 m_offset_position {0.0F, 0.0F};

            // Origin of the element relative to its own size.
            //
            // (0.0, 0.0) - top left corner
            // (0.5, 0.5) - center
            // (1.0, 0.5) - right center
            // (1.0, 1.0) - bottom right corner
            glm::vec2 m_origin {0.0F, 0.0F};

            // Absolute Position. (calculated)
            //
            // Absolute position of the element in pixels, relative to the top-left corner of the screen.
            glm::vec2 m_absolute_position { 0.0F, 0.0F};

            // Absolute Size. (calculated)
            //
            // Absolute size of the element in pixels.
            glm::vec2 m_absolute_size {0.0F, 0.0F};

            // Fixed Size. Used to override the calculated size.
            glm::vec2 m_fixed_size {};

            // Set of child elements.
            std::vector<std::unique_ptr<Element>> m_children;

            // on hover enter callback
            HoverCallback_t m_hover_enter_callback;

            // on hover exit callback
            HoverCallback_t m_hover_exit_callback;

            //
            MouseButtonCallback_t m_on_press_callback;
            MouseButtonCallback_t m_on_release_callback;
    };
}