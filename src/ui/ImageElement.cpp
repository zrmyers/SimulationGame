#/**
 * @file ImageElement.cpp
 * @brief Implementation of `UI::ImageElement` (sprite rendering helpers).
 */

#include "ImageElement.hpp"

#include "components/Sprite.hpp"
#include "components/Transform.hpp"
#include "ecs/ECS.hpp"

//----------------------------------------------------------------------------------------------------------------------
// Image Element

UI::ImageElement::ImageElement() {
}

UI::ImageElement& UI::ImageElement::SetTexture(std::shared_ptr<Graphics::Texture2D> p_texture) {
    m_p_texture = std::move(p_texture);
    return *this;
}

void UI::ImageElement::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) {

    if (!m_entity.IsValid()) {
        m_entity = ECS::Entity(registry);
    }

    Components::Sprite& sprite = m_entity.FindOrEmplaceComponent<Components::Sprite>();
    sprite.color = {1.0F, 1.0F, 1.0F, 1.0F};
    sprite.texture = m_p_texture;
    sprite.topLeftUV = {0.0F, 0.0F};
    sprite.bottomRightUV = {1.0F, 1.0F};
    sprite.layer = Components::RenderLayer::LAYER_GUI;
    sprite.draw_order = depth;

    glm::vec2 scale = GetAbsoluteSize() / screenSize;

    // get the topleft corner position in screen space
    glm::vec2 spriteTopLeft = glm::vec2(-1.0F, 1.0F) * scale;

    // get absolute position in screen space
    glm::vec2 screenPos = (GetAbsolutePosition() / screenSize) * 2.0F - 1.0F;
    screenPos.y *= -1.0F;

    Components::Transform& transform = m_entity.FindOrEmplaceComponent<Components::Transform>();
        transform
            .Set(glm::mat4(1.0F))
            .Translate({ screenPos - spriteTopLeft, 0.0F})
            .Scale({scale, 1.0F});

    depth++;

    for (auto& p_child : GetChildren()) {
        p_child->UpdateGraphics(registry, screenSize, depth);
    }
}

void UI::ImageElement::ClearGraphics() {
    if (m_entity.IsValid()) {

        // best way to reset is to clear the entity entirely.
        m_entity = ECS::Entity();
    }

    for (auto& p_child : GetChildren()) {
        p_child->ClearGraphics();
    }
}
