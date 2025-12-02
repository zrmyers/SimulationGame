#include "UI.hpp"
#include "components/Renderable.hpp"
#include "components/Sprite.hpp"
#include "components/Text.hpp"
#include "components/Transform.hpp"
#include "core/Logger.hpp"
#include "ecs/ECS.hpp"
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>


UI::Element& UI::Element::SetOrigin(glm::vec2 origin) {
    m_origin = origin;

    return *this;
}

UI::Element& UI::Element::SetRelativeSize(glm::vec2 relative_size) {
    m_relative_size = relative_size;

    return *this;
}

UI::Element& UI::Element::SetRelativePosition(glm::vec2 relative_position) {
    m_relative_position = relative_position;

    return *this;
}

UI::Element& UI::Element::SetOffsetSize(glm::vec2 offset_size) {

    m_offset_size = offset_size;
    return *this;
}

UI::Element& UI::Element::SetOffsetPosition(glm::vec2 offset_position) {

    m_offset_position = offset_position;
    return *this;
}

UI::Element& UI::Element::SetFixedSize(glm::vec2 fixed_size) {
    m_fixed_size = fixed_size;
    return *this;
}

UI::Element& UI::Element::SetLayoutMode(LayoutMode mode) {
    m_layout_mode = mode;
    return *this;
}

glm::vec2 UI::Element::GetOrigin() const {

    return m_origin;
}

glm::vec2 UI::Element::GetRelativeSize() const {
    return m_relative_size;
}

glm::vec2 UI::Element::GetRelativePosition() const {
    return m_relative_position;
}

glm::vec2 UI::Element::GetOffsetSize() const {
    return m_offset_size;
}

glm::vec2 UI::Element::GetOffsetPosition() const {
    return m_offset_position;
}

glm::vec2 UI::Element::GetFixedSize() const {
    return m_fixed_size;
}

UI::LayoutMode UI::Element::GetLayoutMode() const {
    return m_layout_mode;
}

void UI::Element::CalculateSize(glm::vec2 parent_size) {

    if (m_layout_mode == LayoutMode::RELATIVE_TO_PARENT) {
        // Calculate own size first.
        SetAbsoluteSize(parent_size * GetRelativeSize()  + GetOffsetSize());

        // Then calculate children.
        for (auto& p_child : m_children) {
            p_child->CalculateSize(GetAbsoluteSize());
        }
    }
    else if (m_layout_mode == LayoutMode::FIXED) {

        SetAbsoluteSize(m_fixed_size);

        // Then calculate children.
        for (auto& p_child : m_children) {
            p_child->CalculateSize(GetAbsoluteSize());
        }
    }
}

void UI::Element::CalculatePosition(glm::vec2 parent_size, glm::vec2 parent_position) {

    // use relative position to calculate own position.
    SetAbsolutePosition(parent_position
        + parent_size * GetRelativePosition()
        + GetOffsetPosition()
        + GetOrigin() * GetAbsoluteSize());

    // update child positions
    for (auto& p_child : GetChildren()) {
        p_child->CalculatePosition(GetAbsoluteSize(), GetAbsolutePosition());
    }
}


glm::vec2 UI::Element::GetAbsoluteSize() const {
    return m_absolute_size;
}

glm::vec2 UI::Element::GetAbsolutePosition() const {
    return m_absolute_position;
}

void UI::Element::SetAbsoluteSize(glm::vec2 size) {
    m_absolute_size = size;
}

void UI::Element::SetAbsolutePosition(glm::vec2 position) {
    m_absolute_position = position;
}

void UI::Element::AddChild(std::unique_ptr<Element>&& child) {
    m_children.emplace_back(std::move(child));
}

std::vector<std::unique_ptr<UI::Element>>& UI::Element::GetChildren() {
    return m_children;
}

//----------------------------------------------------------------------------------------------------------------------
// Horizontal Layout Element
UI::HorizontalLayout::HorizontalLayout() {
    SetLayoutMode(LayoutMode::FIT_TO_CHILDREN);
}

void UI::HorizontalLayout::CalculateSize(glm::vec2 parent_size) {

    uint32_t num_fixed = 0U;
    uint32_t num_relative = 0U;
    // iterate through each child, processing children with fixed size layout first.
    glm::vec2 totalSize = {0.0F, 0.0F};

    for (auto& p_child : GetChildren()) {

        if (p_child->GetLayoutMode() == LayoutMode::FIXED) {
            p_child->CalculateSize(parent_size);
            glm::vec2 childFixedSize = p_child->GetAbsoluteSize();
            totalSize.x += childFixedSize.x;
            totalSize.y = std::max(totalSize.y, childFixedSize.y);
            num_fixed++;
        }
        else {
            num_relative++;
        }
    }

    if (num_relative > 0) {

        // divide up remaining space among relative sized children.
        glm::vec2 horizontalPartitionSize
            = glm::vec2((parent_size.x - totalSize.x)/static_cast<float>(num_relative)
                , std::max(parent_size.y, totalSize.y));
        // calculate size of children.
        for (auto& p_child : GetChildren()) {

            horizontalPartitionSize.x = std::max(horizontalPartitionSize.x, 0.0F);
            if (p_child->GetLayoutMode() != LayoutMode::FIXED) {
                p_child->CalculateSize(horizontalPartitionSize);
                glm::vec2 childSize = p_child->GetAbsoluteSize();
                totalSize.x += childSize.x;
                totalSize.y = std::max(childSize.y, totalSize.y);
            }
        }
    }

    SetAbsoluteSize(totalSize);

}

void UI::HorizontalLayout::CalculatePosition(glm::vec2 parent_size, glm::vec2 parent_position) {

    Element::CalculatePosition(parent_size, parent_position);
    glm::vec2 currentPos = GetAbsolutePosition();

    for (auto& p_child : GetChildren()) {

        glm::vec2 childAbsSize = p_child->GetAbsoluteSize();
        p_child->CalculatePosition(childAbsSize, currentPos);

        currentPos.x += childAbsSize.x;
    }
}

void UI::HorizontalLayout::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) {

    depth++;
    for (auto& p_child : GetChildren()) {
        p_child->UpdateGraphics(registry, screenSize, depth);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Vertical Layout Element
UI::VerticalLayout::VerticalLayout() {
    SetLayoutMode(LayoutMode::FIT_TO_CHILDREN);
}

void UI::VerticalLayout::CalculateSize(glm::vec2 parent_size) {

    uint32_t num_fixed = 0U;
    uint32_t num_relative = 0U;
    // iterate through each child, processing children with fixed size layout first.
    glm::vec2 totalSize = {0.0F, 0.0F};

    for (auto& p_child : GetChildren()) {

        if (p_child->GetLayoutMode() == LayoutMode::FIXED) {
            p_child->CalculateSize(parent_size);
            glm::vec2 childFixedSize = p_child->GetAbsoluteSize();
            totalSize.y += childFixedSize.y;
            totalSize.x = std::max(totalSize.x, childFixedSize.x);
            num_fixed++;
        }
        else {
            num_relative++;
        }
    }

    if (num_relative > 0) {

        // divide up remaining space among relative sized children.
        glm::vec2 verticalPartitionSize
            = glm::vec2(std::max(parent_size.x, totalSize.x),
             (parent_size.y - totalSize.y)/static_cast<float>(num_relative));
        // calculate size of children.
        for (auto& p_child : GetChildren()) {

            verticalPartitionSize.y = std::max(verticalPartitionSize.y, 0.0F);
            if (p_child->GetLayoutMode() != LayoutMode::FIXED) {
                p_child->CalculateSize(verticalPartitionSize);
                glm::vec2 childSize = p_child->GetAbsoluteSize();
                totalSize.y += childSize.y;
                totalSize.x = std::max(childSize.x, totalSize.x);
            }
        }
    }

    SetAbsoluteSize(totalSize);
}

void UI::VerticalLayout::CalculatePosition(glm::vec2 parent_size, glm::vec2 parent_position) {
    Element::CalculatePosition(parent_size, parent_position);
    glm::vec2 currentPos = GetAbsolutePosition();

    for (auto& p_child : GetChildren()) {

        glm::vec2 childAbsSize = p_child->GetAbsoluteSize();
        p_child->CalculatePosition(childAbsSize, currentPos);

        currentPos.y += childAbsSize.y;
    }
}

void UI::VerticalLayout::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) {
    depth++;
    for (auto& p_child : GetChildren()) {
        p_child->UpdateGraphics(registry, screenSize, depth);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Image Element

UI::ImageElement::ImageElement() {
}

UI::ImageElement& UI::ImageElement::SetTexture(std::shared_ptr<SDL::GpuTexture> p_texture) {
    m_p_texture = std::move(p_texture);
    return *this;
}

UI::ImageElement& UI::ImageElement::SetSampler(std::shared_ptr<SDL::GpuSampler> p_sampler) {
    m_p_sampler = std::move(p_sampler);
    return *this;
}


void UI::ImageElement::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) {

    if (!m_entity.IsValid()) {
        m_entity = ECS::Entity(registry);
    }

    Components::Sprite& sprite = m_entity.FindOrEmplaceComponent<Components::Sprite>();
    sprite.color = {1.0F, 1.0F, 1.0F, 1.0F};
    sprite.texture = m_p_texture;
    sprite.sampler = m_p_sampler;
    sprite.topLeftUV = {0.0F, 0.0F};
    sprite.bottomRightUV = {1.0F, 1.0F};
    sprite.layer = Components::RenderLayer::LAYER_GUI;

    glm::vec2 center = screenSize / 2.0F;
    glm::vec2 translate = center - GetAbsolutePosition();

    // translate needs to be changed from pixel coordinate to screen coordinate. [0, resX] -> [-1.0, 1.0]
    translate /= screenSize;
    translate *= 2.0F;
    translate.x *= -1.0F;

    glm::vec2 scale = GetAbsoluteSize() / screenSize;

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

//----------------------------------------------------------------------------------------------------------------------
// Text Element

UI::TextElement::TextElement()
    : m_color(1.0F, 1.0F, 1.0F, 1.0F) {
}

UI::TextElement& UI::TextElement::SetFont(std::shared_ptr<SDL::TTF::Font> p_font) {
    m_p_font = std::move(p_font);
    return *this;
}

UI::TextElement& UI::TextElement::SetText(std::shared_ptr<SDL::TTF::Text> p_text) {
    m_p_text = std::move(p_text);
    return *this;
}

UI::TextElement& UI::TextElement::SetTextString(const std::string& str) {
    if (m_p_text == nullptr) {
        throw std::runtime_error(
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
        throw std::runtime_error(
            "UI::TextElement::GetTextSize(): Attempt to get size of text when m_p_text is not initialized.");
    }
    int width = 0;
    int height = 0;
    m_p_text->GetSize(width,height);

    return glm::vec2(width, height);
}

void UI::TextElement::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) {

    if (!m_entity.IsValid()) {
        m_entity = ECS::Entity(registry);
    }

    Components::Text& textComponent = m_entity.FindOrEmplaceComponent<Components::Text>();
    textComponent.m_p_text = m_p_text;
    textComponent.m_p_font = m_p_font;
    textComponent.m_color = m_color;
    textComponent.m_layer = Components::RenderLayer::LAYER_GUI;

    glm::vec2 center = screenSize / 2.0F;
    glm::vec2 translate = center - GetAbsolutePosition();

    // translate needs to be changed from pixel coordinate to screen coordinate. [0, resX] -> [-1.0, 1.0]
    translate /= screenSize;
    translate *= 2.0F;
    translate.x *= -1.0F;

    // quads produced for the font are already in units of pixels, so just need to be scaled to screen.
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