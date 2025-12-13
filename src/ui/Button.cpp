#include "Button.hpp"

//----------------------------------------------------------------------------------------------------------------------
// Button

UI::Button::Button()
    : m_current_state(ButtonState::UNKNOWN)
    , m_p_frame(nullptr)
    , m_p_text(nullptr) {

    SetHoverEnterCallback([this](){
        if (this->m_current_state == ButtonState::ENABLED) {
            this->SetButtonState(ButtonState::FOCUSED);
        }
    });

    SetHoverExitCallback([this]() {
        if (this->m_current_state == ButtonState::FOCUSED || this->m_current_state == ButtonState::ACTIVATED) {
            this->SetButtonState(ButtonState::ENABLED);
        }
    });

    SetMouseButtonPressCallback([this](MouseButtonID button){

        if ((this->m_current_state == ButtonState::ENABLED) || (this->m_current_state == ButtonState::FOCUSED)
            && (button == MouseButtonID::MOUSE_LEFT)) {
            this->SetButtonState(ButtonState::ACTIVATED);
        }
    });

    SetMouseButtonReleaseCallback([this](MouseButtonID button){

        if ((this->m_current_state == ButtonState::ACTIVATED) && (button == MouseButtonID::MOUSE_LEFT)) {
            this->SetButtonState(ButtonState::FOCUSED);

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

        m_current_state = state;
    }

    return *this;
}

// Set button state change callback.
UI::Button& UI::Button::SetOnClickCallback(UI::OnClickCallback callback) {
    m_click_callback = std::move(callback);
    return *this;
}

void UI::Button::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) {

    depth++;
    for (auto& p_child : GetChildren()) {
        p_child->UpdateGraphics(registry, screenSize, depth);
    }
}
