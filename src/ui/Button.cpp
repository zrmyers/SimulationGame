#/**
 * @file Button.cpp
 * @brief Implementation of UI button widget.
 */

#include "Button.hpp"
#include "ButtonStyle.hpp"

//----------------------------------------------------------------------------------------------------------------------
// Button

UI::Button::Button()
    : m_current_state(ButtonState::UNKNOWN)
    , m_selected(false)
    , m_p_frame(nullptr)
    , m_p_text(nullptr) {

    SetHoverEnterCallback([this](){
        switch (m_current_state) {

            case ButtonState::ENABLED:
                this->SetButtonState(ButtonState::FOCUSED);
                break;

            case ButtonState::UNKNOWN:
            case ButtonState::DISABLED:
            case ButtonState::FOCUSED:
            case ButtonState::ACTIVATED:
            case ButtonState::SELECTED:
                break;
        }
    });

    SetHoverExitCallback([this]() {
        switch (m_current_state) {

            case ButtonState::FOCUSED:
            case ButtonState::ACTIVATED:
                SetButtonState(m_selected? ButtonState::SELECTED : ButtonState::ENABLED);
                break;

            case ButtonState::UNKNOWN:
            case ButtonState::DISABLED:
            case ButtonState::ENABLED:
            case ButtonState::SELECTED:
                break;
        }
    });

    SetMouseButtonPressCallback([this](MouseButtonID button){

        switch(m_current_state) {

            case ButtonState::ENABLED:
            case ButtonState::FOCUSED:
            case ButtonState::SELECTED:
                this->SetButtonState(ButtonState::ACTIVATED);
                break;

            case ButtonState::UNKNOWN:
            case ButtonState::DISABLED:
            case ButtonState::ACTIVATED:
                break;
        }
    });

    SetMouseButtonReleaseCallback([this](MouseButtonID button){

        if ((this->m_current_state == ButtonState::ACTIVATED) && (button == MouseButtonID::MOUSE_LEFT)) {

            this->SetButtonState(m_selected? ButtonState::SELECTED : ButtonState::FOCUSED);

            // click is registered!
            if (m_click_callback) {
                m_click_callback();
            }
        }
    });
}

UI::Button& UI::Button::SetButtonStyle(std::shared_ptr<ButtonStyle> p_style) {
    m_p_button_style = std::move(p_style);
    return *this;
}

UI::Button& UI::Button::SetText(const std::string& text) {
    m_button_text = text;
    if (m_p_text != nullptr) {
        m_p_text->SetTextString(m_button_text);
    }
    return *this;
}

// Changes the button state to a new button state.
UI::Button& UI::Button::SetButtonState(ButtonState state) {

    if ((m_current_state != state) && (m_p_button_style != nullptr)) {

        if (m_current_state == ButtonState::UNKNOWN) {

            // need to initialize elements
            UI::NineSlice& frame = EmplaceChild<UI::NineSlice>();
            m_p_frame = &frame;

            UI::TextElement& text = frame.EmplaceChild<UI::TextElement>();
            text.SetFont(m_p_button_style->GetFont());
            text.SetText(m_p_button_style->GetFont()->CreateText(m_button_text));
            text.SetFixedSize(text.GetTextSize());
            text.SetOrigin({0.5F, 0.5F});
            text.SetLayoutMode(LayoutMode::FIXED);
            text.SetRelativePosition({0.5F, 0.5F});
            m_p_text = &text;
        }

        m_p_frame->SetStyle(m_p_button_style->GetNineSliceStyle(state));
        m_p_text->SetTextColor(m_p_button_style->GetTextColor(state));

        if (state == ButtonState::SELECTED) {
            m_selected = true;
        }
        else if (state == ButtonState::ENABLED) {
            m_selected = false;
        }

        m_current_state = state;
    }

    return *this;
}

// Set button state change callback.
UI::Button& UI::Button::SetOnClickCallback(UI::OnClickCallback callback) {
    m_click_callback = std::move(callback);
    return *this;
}

void UI::Button::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) {

    depth++;
    for (auto& p_child : GetChildren()) {
        p_child->UpdateGraphics(registry, screenSize, depth);
    }
}
