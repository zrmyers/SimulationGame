/**
 * @file RadioStyle.cpp
 * @brief Implementation for radio group style accessor methods.
 */

#include "RadioStyle.hpp"

namespace UI {
RadioStyle::RadioStyle() {
}

void RadioStyle::SetCheckboxStyle(std::shared_ptr<CheckBoxStyle> p_style) {
    m_p_checkbox_style = std::move(p_style);
}

const std::shared_ptr<CheckBoxStyle>& RadioStyle::GetCheckboxStyle() {
    return m_p_checkbox_style;
}

void RadioStyle::SetFont(std::shared_ptr<Graphics::Font> p_font) {
    m_p_font = std::move(p_font);
}

const std::shared_ptr<Graphics::Font>& RadioStyle::GetFont() {
    return m_p_font;
}


}