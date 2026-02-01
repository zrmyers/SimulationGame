/**
 * @file Element.hpp
 * @brief Base UI element declarations and layout utilities.
 */

#pragma once

#include "glm/vec2.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_scancode.h>
#include <functional>
#include <memory>
#include <cstdint>
#include <string>

namespace ECS {
    class Registry;
}

// defines the base elements from which a UI can be built.
namespace UI {

    using Depth_t = uint16_t;

    // upper 8 bits indicate which canvas an element is being drawn for.
    static constexpr Depth_t CANVAS_DEPTH_MASK = 0xFF00U;
    static constexpr uint32_t CANVAS_DEPTH_SHIFT = 8U;

    static constexpr uint8_t GetCanvasDepth(Depth_t depth) {
        return static_cast<uint8_t>((depth & CANVAS_DEPTH_MASK) >> CANVAS_DEPTH_SHIFT);
    }

    static constexpr void SetCanvasDepth(Depth_t& depth, uint8_t canvas_depth) {
        Depth_t value = depth & ~CANVAS_DEPTH_MASK;
        value |= static_cast<Depth_t>(canvas_depth) << CANVAS_DEPTH_SHIFT;
        depth = value;
    }

    /**
     * @enum MouseButtonID
     * @brief Identifiers for mouse buttons used by UI.
     */
    enum class MouseButtonID : uint8_t {
        MOUSE_LEFT = 0,
        MOUSE_RIGHT
    };

    /**
     * @enum LayoutMode
     * @brief Mode describing how an element sizes itself relative to parent or children.
     */
    enum class LayoutMode : uint8_t {
        RELATIVE_TO_PARENT = 0,
        FIT_TO_CHILDREN,
        FIXED
    };

    using HoverCallback_t = std::function<void(void)>;
    using MouseButtonCallback_t = std::function<void(MouseButtonID)>;
    using TextInputCallback_t = std::function<bool(const std::string&)>;
    using KeyboardInputCallback_t = std::function<bool(const SDL_KeyboardEvent&)>;

    /**
     * @class Element
     * @brief Base element for UI widgets providing layout, input callbacks and child management.
     */
    class Element {

        public:
            Element() = default;
            Element(const Element& other) = delete;
            Element(Element&& other) = default;
            Element& operator=(const Element& other) = delete;
            Element& operator=(Element&& other) = default;
            virtual ~Element() = default;

            /**
             * @brief Set the origin point of this element (relative to its size).
             * @param origin Origin expressed in relative coordinates (0..1).
             * @return Reference to this element.
             */
            Element& SetOrigin(glm::vec2 origin);

            /**
             * @brief Set the relative size of this element compared to its parent.
             * @param relative_size Relative size multiplier.
             * @return Reference to this element.
             */
            Element& SetRelativeSize(glm::vec2 relative_size);

            /**
             * @brief Set the relative position of this element within its parent.
             * @param relative_position Relative position (0..1 range).
             * @return Reference to this element.
             */
            Element& SetRelativePosition(glm::vec2 relative_position);

            /**
             * @brief Set an absolute pixel offset to add to the computed size.
             * @param offset_size Pixel offset for size.
             * @return Reference to this element.
             */
            Element& SetOffsetSize(glm::vec2 offset_size);

            /**
             * @brief Set an absolute pixel offset to add to the computed position.
             * @param offset_position Pixel offset for position.
             * @return Reference to this element.
             */
            Element& SetOffsetPosition(glm::vec2 offset_position);

            /**
             * @brief Override the calculated size with a fixed pixel size.
             * @param fixed_size Absolute size in pixels.
             * @return Reference to this element.
             */
            Element& SetFixedSize(glm::vec2 fixed_size);

            /**
             * @brief Set layout mode controlling size calculation behavior.
             * @param mode Layout mode to apply.
             * @return Reference to this element.
             */
            Element& SetLayoutMode(LayoutMode mode);

            /** @brief Get the origin of this element. */
            glm::vec2 GetOrigin() const;

            /** @brief Get the relative size configured for this element. */
            glm::vec2 GetRelativeSize() const;

            /** @brief Get the relative position configured for this element. */
            glm::vec2 GetRelativePosition() const;

            /** @brief Get the offset size in pixels. */
            glm::vec2 GetOffsetSize() const;

            /** @brief Get the offset position in pixels. */
            glm::vec2 GetOffsetPosition() const;

            /** @brief Get the configured fixed size. */
            glm::vec2 GetFixedSize() const;

            /** @brief Get the layout mode for this element. */
            LayoutMode GetLayoutMode() const;

            /**
             * @brief Calculate the size of the element.
             * @param parent_size The absolute size of the parent element.
             */
            virtual void CalculateSize(glm::vec2 parent_size);

            /**
             * @brief Calculate the position of the element.
             * @param parent_size The absolute size of the parent element.
             * @param parent_position The absolute position of the parent element.
             */
            virtual void CalculatePosition(glm::vec2 parent_size, glm::vec2 parent_position);

            /**
             * @brief Process mouse movement and invoke hover callbacks as needed.
             * @param prev_position_px Previous mouse position in pixels.
             * @param current_position_px Current mouse position in pixels.
             */
            void OnHover(glm::vec2 prev_position_px, glm::vec2 current_position_px);

            /**
             * @brief Process mouse press events and route to children or callbacks.
             * @param press_position Press position in pixels.
             * @param button_id Mouse button identifier.
             * @return True if the event was handled.
             */
            bool OnMousePress(glm::vec2 press_position, MouseButtonID button_id);

            /**
             * @brief Process mouse release events and route to children or callbacks.
             * @param release_position Release position in pixels.
             * @param button_id Mouse button identifier.
             * @return True if the event was handled.
             */
            bool OnMouseRelease(glm::vec2 release_position, MouseButtonID button_id);

            /**
             * @brief Process text input events and route to children or callbacks.
             * @param text Input text string.
             */
            bool OnTextInput(const std::string& text);

            /**
             * @brief Process keyboard input events and route to children or callbacks.
             * @param event The keyboard event.
             */
            bool OnKeyboardInput(const SDL_KeyboardEvent& event);

            /**
             * @brief Update the element. If element is visible, updates sprite and text primitives.
             * @param registry ECS registry used for rendering components.
             * @param screenSize The full screen resolution in pixels.
             * @param depth Draw order depth.
             */
            virtual void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) = 0;

            /** @brief Clear Graphics for the given element. */
            virtual void ClearGraphics();

            /** @brief Get the computed absolute size of the element. */
            glm::vec2 GetAbsoluteSize() const;

            /** @brief Get the computed absolute position of the element. */
            glm::vec2 GetAbsolutePosition() const;

            /**
             * @brief Add a child element to this element.
             * @param child Unique pointer to the child element to add.
             */
            void AddChild(std::unique_ptr<Element>&& child);

            template<typename T>
            T& EmplaceChild() {
                std::unique_ptr<Element>& child = m_children.emplace_back(std::make_unique<T>());
                child->m_p_parent = this;
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
            Element& SetTextInputCallback(TextInputCallback_t callback);
            Element& SetKeyboardInputCallback(KeyboardInputCallback_t callback);

            /**
             * @brief Check if any child TextInputBox elements have input processing enabled.
             * @return True if any child TextInputBox is processing input, false otherwise.
             */
            virtual bool IsTextInputEnabled() const;

            // Used by child elements to make it clear that graphics need to be updated.
            void SetDirty();

            // Used by gui system to clear dirty flag after graphics update.
            void ClearDirty();

            // Get whether the given element is dirty.
            bool GetDirty() const;

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

            // Pointer to parent.
            Element* m_p_parent {nullptr};

            // on hover enter callback
            HoverCallback_t m_hover_enter_callback;

            // on hover exit callback
            HoverCallback_t m_hover_exit_callback;

            //
            MouseButtonCallback_t m_on_press_callback;
            MouseButtonCallback_t m_on_release_callback;

            // Text input callback
            TextInputCallback_t m_text_input_callback;

            // Keyboard input callback
            KeyboardInputCallback_t m_keyboard_input_callback;

            // Whether the element is changed since last graphics update.
            bool m_is_dirty {true};
    };
}