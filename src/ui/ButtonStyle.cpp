#include "ButtonStyle.hpp"

//----------------------------------------------------------------------------------------------------------------------
// Button Style

UI::ButtonStyle::ButtonStyle()
    : m_text_colors({glm::vec4(1.0F, 1.0F, 1.0F, 1.0F)}) {
}

void UI::ButtonStyle::SetFont(std::shared_ptr<Graphics::Font> p_font) {
    m_p_text_font = std::move(p_font);
}

const std::shared_ptr<Graphics::Font>& UI::ButtonStyle::GetFont() {
    return m_p_text_font;
}

void UI::ButtonStyle::SetNineSliceStyle(ButtonState state, std::shared_ptr<NineSliceStyle> p_style) {
    m_frame_styles.at(static_cast<size_t>(state)) = std::move(p_style);
}

const std::shared_ptr<UI::NineSliceStyle>& UI::ButtonStyle::GetNineSliceStyle(ButtonState state) {
    return m_frame_styles.at(static_cast<size_t>(state));
}

void UI::ButtonStyle::SetTextColor(ButtonState state, glm::vec4 color) {
    m_text_colors.at(static_cast<size_t>(state)) = color;
}

const glm::vec4& UI::ButtonStyle::GetTextColor(ButtonState state) {
    return m_text_colors.at(static_cast<size_t>(state));
}
