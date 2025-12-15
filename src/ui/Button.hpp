#pragma once

#include "Element.hpp"
#include "ButtonStyle.hpp"
#include "NineSlice.hpp"
#include "TextElement.hpp"

namespace UI {

    //! Callback when a click is detected.
    //!
    //! A click occurs when the left mouse button is pressed and released without leaving the button. A click is
    //! cancelled when the mouse cursor leaves the UI button before the mouse button is released.
    using OnClickCallback = std::function<void()>;

    // Basic button.
    //
    // Basic button registers default handlers for mouse button press and release, as well as detection of hovering.
    class Button : public Element {

        public:

            Button();

            Button& SetButtonStyle(std::shared_ptr<ButtonStyle> p_style);
            Button& SetText(const std::string& text);

            // Changes the button state to a new button state.
            //
            // This must be called after the button style and text have been configured.
            Button& SetButtonState(ButtonState state);

            // Set button state change callback.
            Button& SetOnClickCallback(OnClickCallback callback);

            // main place where graphics are updated.
            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) override;
        private:

            //! Reference to button style
            std::shared_ptr<ButtonStyle> m_p_button_style;

            //! Current button state
            ButtonState m_current_state;

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