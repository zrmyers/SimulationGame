/**
 * @file CheckBox.cpp
 * @brief Implementation of checkbox widget behavior and rendering glue.
 */

#include "CheckBox.hpp"
#include "CheckBoxStyle.hpp"
#include "Element.hpp"
#include "graphics/Texture2D.hpp"
#include "ui/ImageElement.hpp"
#include <memory>
#include <utility>


UI::CheckBox::CheckBox()
    : m_p_image_element(nullptr)
    , m_current_state(CheckBoxState::UNKNOWN)
    , m_disable_off_toggle(false) {

    SetHoverEnterCallback([this](){
        switch (m_current_state) {

            case CheckBoxState::ON:
                SetCheckBoxState(CheckBoxState::ON_FOCUSED);
                break;

            case CheckBoxState::OFF:
                SetCheckBoxState(CheckBoxState::OFF_FOCUSED);
                break;

            case CheckBoxState::OFF_FOCUSED:
            case CheckBoxState::ON_FOCUSED:
            case CheckBoxState::ON_ACTIVATED:
            case CheckBoxState::OFF_ACTIVATED:
            case CheckBoxState::UNKNOWN:
                // Do nothing
                break;
        }
    });

    SetHoverExitCallback([this]() {
        switch (m_current_state) {

            case CheckBoxState::ON_FOCUSED:
                SetCheckBoxState(CheckBoxState::ON);
                break;

            case CheckBoxState::OFF_FOCUSED:
                SetCheckBoxState(CheckBoxState::OFF);
                break;

            case CheckBoxState::ON_ACTIVATED:
                // cancel the click
                SetCheckBoxState(CheckBoxState::ON);
                break;

            case CheckBoxState::OFF_ACTIVATED:
                // cancel the click
                SetCheckBoxState(CheckBoxState::OFF);
                break;

            case CheckBoxState::ON:
            case CheckBoxState::OFF:
            case CheckBoxState::UNKNOWN:
                // Do nothing
                break;
        }
    });

    SetMouseButtonPressCallback([this](MouseButtonID button){

        if (button == MouseButtonID::MOUSE_LEFT) {
            switch(this->m_current_state) {

                case UI::CheckBoxState::ON:
                case UI::CheckBoxState::ON_FOCUSED:
                    SetCheckBoxState(CheckBoxState::ON_ACTIVATED);
                    break;

                case UI::CheckBoxState::OFF:
                case UI::CheckBoxState::OFF_FOCUSED:
                    SetCheckBoxState(CheckBoxState::OFF_ACTIVATED);
                    break;

                case UI::CheckBoxState::UNKNOWN:
                default:
                    break;
            }
        }
    });

    SetMouseButtonReleaseCallback([this](MouseButtonID button){

        if (button == MouseButtonID::MOUSE_LEFT) {
            switch(this->m_current_state) {

                case UI::CheckBoxState::ON_ACTIVATED:
                    if (!m_disable_off_toggle) {
                        SetCheckBoxState(CheckBoxState::OFF_FOCUSED);
                        if (m_state_callback) {
                            m_state_callback(false);
                        }
                    }
                    break;

                case UI::CheckBoxState::OFF_ACTIVATED:
                    SetCheckBoxState(CheckBoxState::ON_FOCUSED);
                    if (m_state_callback) {
                        m_state_callback(true);
                    }
                    break;

                case UI::CheckBoxState::UNKNOWN:
                case CheckBoxState::ON:
                case CheckBoxState::OFF:
                case CheckBoxState::ON_FOCUSED:
                case CheckBoxState::OFF_FOCUSED:
                    // do nothing
                    break;
            }
        }
    });
}

UI::CheckBox& UI::CheckBox::SetStyle(std::shared_ptr<CheckBoxStyle> p_style) {
    m_p_style = std::move(p_style);
    return *this;
}

UI::CheckBox&  UI::CheckBox::SetCheckBoxStateCallback(CheckBoxStateCallback_t callback) {
    m_state_callback = std::move(callback);
    return *this;
}

UI::CheckBox&  UI::CheckBox::SetCheckBoxState(CheckBoxState state) {

    if ((state != m_current_state) && (m_p_style != nullptr)) {

        if (m_current_state == CheckBoxState::UNKNOWN) {

            UI::ImageElement& image = EmplaceChild<UI::ImageElement>();
            image.SetLayoutMode(LayoutMode::RELATIVE_TO_PARENT)
                 .SetOrigin({0.5F, 0.5F})
                 .SetRelativePosition({0.5F, 0.5F});
            m_p_image_element = &image;
        }

        const std::shared_ptr<Graphics::Texture2D>& p_texture = m_p_style->GetImage(state);

        m_p_image_element->SetTexture(p_texture);

        m_current_state = state;
    }

    return *this;
}


UI::CheckBox& UI::CheckBox::SetToggleOffDisabled(bool disabled) {
    m_disable_off_toggle = disabled;
    return *this;
}

void UI::CheckBox::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) {

    depth++;
    for (auto& p_child : GetChildren()) {
        p_child->UpdateGraphics(registry, screenSize, depth);
    }
}