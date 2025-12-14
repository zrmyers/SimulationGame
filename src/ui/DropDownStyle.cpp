#include "DropDownStyle.hpp"


UI::DropDownStyle::DropDownStyle() {
}

void UI::DropDownStyle::SetSelectionButtonStyle(std::shared_ptr<ButtonStyle> p_style) {
    m_p_button_style = std::move(p_style);
}

const std::shared_ptr<UI::ButtonStyle>& UI::DropDownStyle::GetSelectionButtonStyle() {
    return m_p_button_style;
}

void UI::DropDownStyle::SetOptionsButtonStyle(std::shared_ptr<ButtonStyle> p_options) {
    m_p_options_style = std::move(p_options);
}

const std::shared_ptr<UI::ButtonStyle>& UI::DropDownStyle::GetOptionsButtonStyle() {
    return m_p_options_style;
}