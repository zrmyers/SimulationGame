/**
 * @file Button.hpp
 * @brief Button widget declarations for the UI system.
 */

#pragma once

#include "Element.hpp"
#include "ButtonStyle.hpp"
#include "NineSlice.hpp"
#include "TextElement.hpp"

namespace UI {

    /**
     * @brief Callback when a click is detected.
     *
     * A click occurs when the left mouse button is pressed and released without leaving the button. A click is
     * cancelled when the mouse cursor leaves the UI button before the mouse button is released.
     */
    using OnClickCallback = std::function<void()>;

    /**
     * @class Button
     * @brief Basic push-button widget with click handling and text.
     *
     * Button registers default handlers for mouse button press and release, as well as detection of hovering.
     */
    class Button : public Element {

        public:

            /** @brief Default constructor. */
            Button();

            /**
             * @brief Set the button style to use for rendering.
             * @param p_style Shared pointer to a ButtonStyle.
             * @return Reference to this Button.
             */
            Button& SetButtonStyle(std::shared_ptr<ButtonStyle> p_style);

            /**
             * @brief Set the text displayed on the button.
             * @param text The text string to display.
             * @return Reference to this Button.
             */
            Button& SetText(const std::string& text);

            /**
             * @brief Changes the button state to a new button state.
             * @note This must be called after the button style and text have been configured.
             * @param state New button state to apply.
             * @return Reference to this Button.
             */
            Button& SetButtonState(ButtonState state);

            /**
             * @brief Set the onclick callback invoked when the button is clicked.
             * @param callback Function to call on click.
             * @return Reference to this Button.
             */
            Button& SetOnClickCallback(OnClickCallback callback);

            /**
             * @brief Update graphics for the button (called during UI render pass).
             * @param registry ECS registry used for rendering components.
             * @param screenSize The full screen resolution in pixels.
             * @param depth Draw order depth.
             */
            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) override;
        private:

            //! Reference to button style
            std::shared_ptr<ButtonStyle> m_p_button_style;

            //! Current button state
            ButtonState m_current_state;

            //! Whether the button is currently selected
            bool m_selected;

            //! pointer to nine-slice that makes up background of button
            NineSlice* m_p_frame;

            //! pointer to text element
            TextElement* m_p_text;

            //! Function to call when the button state changes.
            OnClickCallback m_click_callback;

            //! button text
            std::string m_button_text;

    };
}