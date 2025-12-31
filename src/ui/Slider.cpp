#include "Slider.hpp"
#include "Element.hpp"
#include "HorizontalLayout.hpp"
#include "NineSlice.hpp"
#include "SliderStyle.hpp"
#include "Spacer.hpp"
#include "TextElement.hpp"
#include "core/Logger.hpp"
#include "graphics/Font.hpp"
#include "ui/ImageElement.hpp"
#include <SDL3/SDL_mouse.h>
#include <algorithm>
#include <cstdlib>
#include <glm/ext/vector_float2.hpp>
#include <memory>

namespace UI {

Slider::Slider()
    : m_state(SliderState::UNKNOWN)
    , m_selected(0U)
    , m_p_filled(nullptr)
    , m_p_unfilled(nullptr)
    , m_p_knob(nullptr)
    , m_p_value_label(nullptr) {

    SetHoverEnterCallback([this](){
        switch (m_state) {

            case UI::SliderState::ENABLED:
                this->SetSliderState(SliderState::FOCUSED);
                break;

            case UI::SliderState::FOCUSED:
            default:
                break;
        }
    });

    SetHoverExitCallback([this]() {
        switch (m_state) {

            case UI::SliderState::FOCUSED:
                if (!m_track_mouse) {
                    this->SetSliderState(SliderState::ENABLED);
                }
                break;

            case UI::SliderState::ENABLED:
            default:
                break;
        }
    });

    SetMouseButtonPressCallback([this](MouseButtonID button){

        m_track_mouse = true;
    });
}

Slider& Slider::SetStyle(const std::shared_ptr<SliderStyle>& p_style) {
    m_p_style = p_style;
    return *this;
}

Slider& Slider::SelectOption(size_t select) {

    if ((m_state != SliderState::UNKNOWN) && (select < m_options.size())) {

        float relativeFillSize = std::clamp(
            static_cast<float>(select)/static_cast<float>(m_options.size() - 1), 0.0F, 1.0F);
        m_p_filled->SetRelativeSize(glm::vec2(relativeFillSize, 1.0F));

        m_p_value_label->SetTextString(m_options.at(select));
        m_p_value_label->SetFixedSize(m_p_value_label->GetTextSize());

    }

    m_selected = select;

    if (m_value_change_callback) {
        m_value_change_callback(select);
    }

    return *this;
}

Slider& Slider::SetOptions(std::vector<std::string> options) {
    m_options = std::move(options);
    return *this;
}

Slider& Slider::SetValueChangedCallback(SliderValueChangeCallback callback) {
    m_value_change_callback = std::move(callback);
    return *this;
}

Slider& Slider::SetSliderState(SliderState state) {

    if ((m_state != state) && (m_p_style != nullptr)) {

        if (m_state == SliderState::UNKNOWN) {

            // layout slider and label horizontally
            UI::HorizontalLayout& horizontal = EmplaceChild<UI::HorizontalLayout>();

            // construct slider
            UI::NineSlice& unfilled = horizontal.EmplaceChild<UI::NineSlice>();
            unfilled.SetLayoutMode(LayoutMode::RELATIVE_TO_PARENT);
            m_p_unfilled = &unfilled;

            UI::NineSlice& filled = unfilled.EmplaceChild<UI::NineSlice>();
            filled.SetLayoutMode(LayoutMode::RELATIVE_TO_PARENT);
            m_p_filled = &filled;

            UI::ImageElement& knob = filled.EmplaceChild<UI::ImageElement>();
            knob.SetFixedSize(glm::vec2(32.0F, 32.0F))
                .SetLayoutMode(LayoutMode::FIXED)
                .SetOrigin({0.5F, 0.5F})
                .SetOffsetPosition({0.0F, 3.0F})
                .SetRelativePosition(glm::vec2(1.0F, 0.0F)); // knob is always on right size of filled zone.
            m_p_knob = &knob;

            const std::shared_ptr<Graphics::Font>& p_font = m_p_style->GetFont();
            UI::TextElement& text = horizontal.EmplaceChild<UI::TextElement>();
            text.SetFont(p_font);
            text.SetText(p_font->CreateText("abcdef"));
            text.SetFixedSize(text.GetTextSize());
            text.SetOrigin({0.5F, 0.5F});
            text.SetLayoutMode(LayoutMode::FIXED);
            text.SetRelativePosition({0.5F, 0.5F});
            m_p_value_label = &text;

            m_state = state;

            if (!m_options.empty()) {
                SelectOption(m_selected);
            }
        }

        m_p_unfilled->SetStyle(m_p_style->GetTrackUnfilledStyle(state));
        m_p_filled->SetStyle(m_p_style->GetTrackFilledStyle(state));
        m_p_knob->SetTexture(m_p_style->GetKnobImage(state));

        m_state = state;
    }

    return *this;
}

void Slider::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) {

    if (m_track_mouse) {
        float mousePositionX = 0.0F;

        SDL_MouseButtonFlags mouseFlags = SDL_GetMouseState(&mousePositionX, nullptr);

        // Check if mouse button is still pressed
        if ((SDL_BUTTON_LMASK & mouseFlags) == 0U) {

            m_track_mouse = false;
            SetSliderState(SliderState::ENABLED);
        } else {
            glm::vec2 sliderAbsPos = m_p_unfilled->GetAbsolutePosition();
            glm::vec2 sliderAbsSize = m_p_unfilled->GetAbsoluteSize();

            // Determine relative position of slider given the current mouse position.
            float relativePosition = std::max(mousePositionX - sliderAbsPos.x, 0.0F);
            relativePosition = std::min(relativePosition / sliderAbsSize.x, 1.0F);

            // convert the relative position to nearest index, and find the nearest index.
            float selectedAsFloat = relativePosition * static_cast<float>(m_options.size() - 1);
            size_t closest = 0;
            float minDifference = static_cast<float>(m_options.size());
            for (size_t index = 0U; index < m_options.size(); index++) {
                float difference = std::abs(selectedAsFloat - static_cast<float>(index));
                if (difference < minDifference) {
                    minDifference = difference;
                    closest = index;
                }
            }

            if (m_selected != closest) {
                SelectOption(closest);
            }
        }
    }

    depth++;
    for (auto& p_child : GetChildren()) {
        p_child->UpdateGraphics(registry, screenSize, depth);
    }
}

}