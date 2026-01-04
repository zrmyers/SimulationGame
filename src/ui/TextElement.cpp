#include <cmath>
#include <cstdint>
#/**
 * @file TextElement.cpp
 * @brief Implementation for `UI::TextElement` rendering and management.
 */

#include "TextElement.hpp"

#include "components/Transform.hpp"
#include "components/Text.hpp"
#include "ecs/ECS.hpp"

//----------------------------------------------------------------------------------------------------------------------
// Text Element

UI::TextElement::TextElement()
    : m_color(1.0F, 1.0F, 1.0F, 1.0F) {
}

UI::TextElement& UI::TextElement::SetFont(std::shared_ptr<Graphics::Font> p_font) {
    m_p_font = std::move(p_font);
    return *this;
}

UI::TextElement& UI::TextElement::SetText(std::shared_ptr<SDL::TTF::Text> p_text) {
    m_p_text = std::move(p_text);
    return *this;
}

UI::TextElement& UI::TextElement::SetTextString(const std::string& str) {
    if (m_p_text == nullptr) {
        throw Core::EngineException(
            "UI::TextElement::SetTextString(): Attempt to set text string when m_p_text is not initialized.");
    }
    m_p_text->SetString(str);
    return *this;
}

UI::TextElement& UI::TextElement::SetTextColor(const glm::vec4& color) {
    m_color = color;
    return *this;
}

glm::vec2 UI::TextElement::GetTextSize() const {
    if (m_p_text == nullptr) {
        throw Core::EngineException(
            "UI::TextElement::GetTextSize(): Attempt to get size of text when m_p_text is not initialized.");
    }
    int width = 0;
    int height = 0;
    m_p_text->GetSize(width,height);

    return glm::vec2(width, height);
}


glm::vec2 UI::TextElement::MeasureStringSize(const std::string& string) const {
    if (m_p_font == nullptr) {
        throw Core::EngineException(
            "UI::TextElement::GetCharacterOffset(): Attempt to get character offset when m_p_text is not initialized.");
    }

    glm::vec2 size(0.0F);
    for (char ascii : string) {
        uint16_t ascii_u16 = static_cast<uint16_t>(ascii);
        glm::vec2 min = {};
        glm::vec2 max = {};
        float advance = 0.0F;
        m_p_font->GetGlyphMetrics(ascii_u16, min, max, advance);
        size.x += advance;
        size.y = std::max(size.y, max.y - min.y);
    }

    return size;
}

void UI::TextElement::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) {

    if (!m_entity.IsValid()) {
        m_entity = ECS::Entity(registry);
    }

    Components::Text& textComponent = m_entity.FindOrEmplaceComponent<Components::Text>();
    textComponent.m_p_text = m_p_text;
    textComponent.m_p_font = m_p_font;
    textComponent.m_color = m_color;
    textComponent.m_layer = Components::RenderLayer::LAYER_GUI;
    textComponent.m_draw_order = depth;

    glm::vec2 center = screenSize / 2.0F;
    glm::vec2 translate = center - GetAbsolutePosition();

    // translate needs to be changed from pixel coordinate to screen coordinate. [0, resX] -> [-1.0, 1.0]
    translate /= screenSize;
    translate *= 2.0F;
    translate.x *= -1.0F;

    // quads produced for the font are already in units of pixels, so just need to be scaled to screen coordinate.
    glm::vec2 scale = 1.0F / screenSize;

    Components::Transform& transform = m_entity.FindOrEmplaceComponent<Components::Transform>();
        transform
            .Set(glm::mat4(1.0F))
            .Translate({translate, 0.0F})
            .Scale({scale, 1.0F});

    depth++;

    for (auto& p_child : GetChildren()) {
        p_child->UpdateGraphics(registry, screenSize, depth);
    }
}

void UI::TextElement::ClearGraphics() {

    if (m_entity.IsValid()) {
        m_entity = ECS::Entity();
    }

    for (auto& p_child : GetChildren()) {
        p_child->ClearGraphics();
    }
}