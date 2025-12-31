#include "SliderStyle.hpp"
#include "core/Engine.hpp"
#include <string>

namespace UI {

std::string ToString(SliderState state) {

    switch (state) {

        case UI::SliderState::ENABLED:
            return "ENABLED";
            break;

        case UI::SliderState::FOCUSED:
            return "FOCUSED";
            break;

        default:
            return std::to_string(static_cast<uint32_t>(state));
            break;
    }
}

void SliderStyle::SetFont(const std::shared_ptr<Graphics::Font>& p_font) {
    m_p_font = p_font;
}

const std::shared_ptr<Graphics::Font>& SliderStyle::GetFont() const {
    return m_p_font;
}

void SliderStyle::SetTrackFilledStyle(SliderState state, const std::shared_ptr<NineSliceStyle>& p_style) {
    m_track_filled_style[state] = p_style;
}

const std::shared_ptr<NineSliceStyle>& SliderStyle::GetTrackFilledStyle(SliderState state) const {
    auto styleIter = m_track_filled_style.find(state);
    if (styleIter == m_track_filled_style.end()) {
        throw Core::EngineException("GetTrackFilledStyle() failed to find style for given state: " + ToString(state));
    }
    return styleIter->second;
}

void SliderStyle::SetTrackUnfilledStyle(SliderState state, const std::shared_ptr<NineSliceStyle>& p_style) {
    m_track_unfilled_style[state] = p_style;
}

const std::shared_ptr<NineSliceStyle>& SliderStyle::GetTrackUnfilledStyle(SliderState state) const {
    auto styleIter = m_track_unfilled_style.find(state);
    if (styleIter == m_track_unfilled_style.end()) {
        throw Core::EngineException("GetTrackUnfilledStyle() failed to find style for given state: " + ToString(state));
    }
    return styleIter->second;
}

void SliderStyle::SetKnobImage(SliderState state, const std::shared_ptr<Graphics::Texture2D>& p_image) {
    m_knob_style[state] = p_image;
}

const std::shared_ptr<Graphics::Texture2D>& SliderStyle::GetKnobImage(SliderState state) const {

    auto styleIter = m_knob_style.find(state);
    if (styleIter == m_knob_style.end()) {
        throw Core::EngineException("GetKnobImage() failed to find style for given state: " + ToString(state));
    }
    return styleIter->second;
}

}