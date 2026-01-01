/**
 * @file CheckBoxStyle.cpp
 * @brief Implementation for checkbox style container.
 */

#include "CheckBoxStyle.hpp"
#include <cstddef>


UI::CheckBoxStyle::CheckBoxStyle() {
}

void UI::CheckBoxStyle::SetImage(CheckBoxState state, std::shared_ptr<Graphics::Texture2D> p_style) {
    m_checkbox_images.at(static_cast<size_t>(state)) = std::move(p_style);
}

const std::shared_ptr<Graphics::Texture2D>& UI::CheckBoxStyle::GetImage(CheckBoxState state) {
    return m_checkbox_images.at(static_cast<size_t>(state));
}