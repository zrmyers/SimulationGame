#include "TextInputBoxStyle.hpp"
#include "core/Engine.hpp"
#include <string>

namespace UI {

TextInputBoxStyle::TextInputBoxStyle()
    : m_default_text_color(0.75F, 0.75F, 0.75F, 1.0F) // NOLINT
    , m_normal_text_color(1.0F, 1.0F, 1.0F, 1.0F)
{
}

void TextInputBoxStyle::SetBoxStyle(TextInputState state, std::shared_ptr<UI::NineSliceStyle> style) {

    m_p_box_style[state] = std::move(style);
}

std::shared_ptr<UI::NineSliceStyle> TextInputBoxStyle::GetBoxStyle(TextInputState state) const {
    auto boxIter = m_p_box_style.find(state);
    if (boxIter != m_p_box_style.end()) {
        return boxIter->second;
    }
    throw Core::EngineException("GetBoxStyle() could not find style for state.");
}

void TextInputBoxStyle::SetTextFont(std::shared_ptr<Graphics::Font> font) {
    m_p_text_font = std::move(font);
}

std::shared_ptr<Graphics::Font> TextInputBoxStyle::GetTextFont() const {
    return m_p_text_font;
}

void TextInputBoxStyle::SetDefaultTextColor(const glm::vec4& color) {
    m_default_text_color = color;
}

glm::vec4 TextInputBoxStyle::GetDefaultTextColor() const {
    return m_default_text_color;
}

void TextInputBoxStyle::SetNormalTextColor(const glm::vec4& color) {
    m_normal_text_color = color;
}

glm::vec4 TextInputBoxStyle::GetNormalTextColor() const {
    return m_normal_text_color;
}

void TextInputBoxStyle::SetCaretStyle(std::shared_ptr<UI::NineSliceStyle> style) {
    m_p_caret_style = std::move(style);
}

std::shared_ptr<UI::NineSliceStyle> TextInputBoxStyle::GetCaretStyle() const {
    return m_p_caret_style;
}

}