#include "UI.hpp"
#include "components/Renderable.hpp"
#include "components/Sprite.hpp"
#include "components/Text.hpp"
#include "components/Transform.hpp"
#include "core/AssetLoader.hpp"
#include "core/Engine.hpp"
#include "core/Logger.hpp"
#include "ecs/ECS.hpp"
#include "graphics/Texture2D.hpp"
#include "sdl/SDL.hpp"
#include "sdl/TTF.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/TextSystem.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <stdexcept>
#include <vector>

#include <nlohmann/json.hpp>

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
        - GetOrigin() * GetAbsoluteSize());

    // update child positions
    for (auto& p_child : GetChildren()) {
        p_child->CalculatePosition(GetAbsoluteSize(), GetAbsolutePosition());
    }
}

void UI::Element::OnHover(glm::vec2 prev_position_px, glm::vec2 current_position_px) {

    bool prevCollision = CheckCollision(prev_position_px);
    bool currentCollision = CheckCollision(current_position_px);

    if (prevCollision || currentCollision) {

        if (m_hover_enter_callback && !prevCollision && currentCollision) {
            m_hover_enter_callback();
        }

        if (m_hover_exit_callback && prevCollision && !currentCollision) {
            m_hover_exit_callback();
        }

        for (auto& p_child : m_children) {

            p_child->OnHover(prev_position_px, current_position_px);
        }
    }
}

void UI::Element::OnMousePress(glm::vec2 press_position, MouseButtonID button_id) {

    if (CheckCollision(press_position)) {

        if (m_on_press_callback) {
            m_on_press_callback(button_id);
        }

        for (auto& p_child : m_children) {
            p_child->OnMousePress(press_position, button_id);
        }
    }
}

void UI::Element::OnMouseRelease(glm::vec2 release_position, MouseButtonID button_id) {

    if (CheckCollision(release_position)) {

        if (m_on_release_callback) {
            m_on_release_callback(button_id);
        }

        for (auto& p_child : m_children) {
            p_child->OnMouseRelease(release_position, button_id);
        }
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

bool UI::Element::CheckCollision(glm::vec2 point_px) const {
    return ((point_px.x >= m_absolute_position.x)
        && (point_px.x <= (m_absolute_position.x + m_absolute_size.x))
        && (point_px.y >= m_absolute_position.y)
        && (point_px.y <= (m_absolute_position.y + m_absolute_size.y)));
}

UI::Element& UI::Element::SetHoverEnterCallback(HoverCallback_t callback) {
    m_hover_enter_callback = std::move(callback);
    return *this;
}

UI::Element& UI::Element::SetHoverExitCallback(HoverCallback_t callback) {
    m_hover_exit_callback = std::move(callback);
    return *this;
}

UI::Element& UI::Element::SetMouseButtonPressCallback(MouseButtonCallback_t callback) {
    m_on_press_callback = std::move(callback);
    return *this;
}

UI::Element& UI::Element::SetMouseButtonReleaseCallback(MouseButtonCallback_t callback) {
    m_on_release_callback = std::move(callback);
    return *this;
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

UI::ImageElement& UI::ImageElement::SetTexture(std::shared_ptr<Graphics::Texture2D> p_texture) {
    m_p_texture = std::move(p_texture);
    return *this;
}

void UI::ImageElement::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) {

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

    // now calculate difference between

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

void UI::TextElement::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) {

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

//----------------------------------------------------------------------------------------------------------------------
// Nine Slice Style

std::shared_ptr<UI::NineSliceStyle> UI::NineSliceStyle::Load(Core::Engine& engine, const std::vector<std::string>& images) {

    Core::AssetLoader& assetLoader = engine.GetAssetLoader();
    UI::NineSliceStyle style;

    if (images.size() != NineSliceStyle::SLICE_COUNT) {
        throw Core::EngineException("Not enough data to initialize nine-slice. Need 9 image files to work.");
    }

    style.m_textures.resize(images.size());

    Systems::RenderSystem& renderSystem = engine.GetEcsRegistry().GetSystem<Systems::RenderSystem>();
    std::shared_ptr<SDL::GpuSampler> p_cornerSampler;
    std::shared_ptr<SDL::GpuSampler> p_horizontalSampler;
    std::shared_ptr<SDL::GpuSampler> p_verticalSampler;
    std::shared_ptr<SDL::GpuSampler> p_fillSampler;

    // Create samplers
    SDL_GPUSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    samplerCreateInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerCreateInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.enable_anisotropy = true;
    samplerCreateInfo.max_anisotropy = 16; // NOLINT

    p_cornerSampler = std::make_shared<SDL::GpuSampler>(renderSystem.CreateSampler(samplerCreateInfo));
    samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    p_horizontalSampler = std::make_shared<SDL::GpuSampler>(renderSystem.CreateSampler(samplerCreateInfo));
    samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    p_verticalSampler = std::make_shared<SDL::GpuSampler>(renderSystem.CreateSampler(samplerCreateInfo));
    samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    p_fillSampler = std::make_shared<SDL::GpuSampler>(renderSystem.CreateSampler(samplerCreateInfo));

    // now create images
    for (size_t index = 0; index < images.size(); index++) {

        Region region = static_cast<Region>(index);
        SDL::Image image(assetLoader.GetImageDir() + "/" + images.at(index));
        std::shared_ptr<Graphics::Texture2D> p_texture = nullptr;
        switch (region) {
            case Region::TOP_LEFT_CORNER:
            case Region::BOTTOM_LEFT_CORNER:
            case Region::TOP_RIGHT_CORNER:
            case Region::BOTTOM_RIGHT_CORNER:
                p_texture = std::make_shared<Graphics::Texture2D>(
                    engine, p_cornerSampler, image.GetWidth(), image.GetHeight(), false);
                break;

            case Region::LEFT_EDGE:
            case Region::RIGHT_EDGE:
                p_texture = std::make_shared<Graphics::Texture2D>(
                    engine, p_verticalSampler, image.GetWidth(), image.GetHeight(), false);
                break;

            case Region::TOP_EDGE:
            case Region::BOTTOM_EDGE:
                p_texture = std::make_shared<Graphics::Texture2D>(
                    engine, p_horizontalSampler, image.GetWidth(), image.GetHeight(), false);
                break;

            case Region::CENTER:
                p_texture = std::make_shared<Graphics::Texture2D>(
                    engine, p_fillSampler, image.GetWidth(), image.GetHeight(), false);
                break;

            default:
                break;
        }

        p_texture->LoadImageData( image);

        style.m_textures[index] = std::move(p_texture);
    }

    style.m_border_width = style.CalculateBorderWidth();

    return std::make_shared<NineSliceStyle>(std::move(style));
}

UI::NineSliceStyle::NineSliceStyle()
    : m_border_width(0.0F) {
    m_textures.resize(NineSliceStyle::SLICE_COUNT);
}

void UI::NineSliceStyle::SetRegion(std::shared_ptr<Graphics::Texture2D> p_texture, Region region) {

    size_t index = static_cast<size_t>(region);

    m_textures.at(index) = std::move(p_texture);
}

const std::shared_ptr<Graphics::Texture2D>& UI::NineSliceStyle::GetRegion(Region region) const {

    size_t index = static_cast<size_t>(region);

    return m_textures.at(index);
}

float UI::NineSliceStyle::GetBorderWidth() const {
    return m_border_width;
}

float UI::NineSliceStyle::CalculateBorderWidth() {

    uint32_t minBorderWidth = 0;

    for (size_t index = 0; index < m_textures.size(); index++) {

        std::shared_ptr<Graphics::Texture2D>& p_texture = m_textures.at(index);
        Region region = static_cast<Region>(index);

        switch (region) {

            case Region::TOP_LEFT_CORNER:
            case Region::BOTTOM_LEFT_CORNER:
            case Region::TOP_RIGHT_CORNER:
            case Region::BOTTOM_RIGHT_CORNER:
                minBorderWidth = std::max({minBorderWidth, p_texture->GetWidth(), p_texture->GetHeight()});
                break;

            case Region::LEFT_EDGE:
            case Region::RIGHT_EDGE:
                minBorderWidth = std::max(minBorderWidth, p_texture->GetWidth());
                break;

            case Region::TOP_EDGE:
            case Region::BOTTOM_EDGE:
                minBorderWidth = std::max(minBorderWidth, p_texture->GetHeight());
                break;

            case Region::CENTER:
            default:
                break;
        }
    }
    return static_cast<float>(minBorderWidth);
}

//----------------------------------------------------------------------------------------------------------------------
// Nine Slice Element

UI::NineSlice& UI::NineSlice::SetStyle(const std::shared_ptr<NineSliceStyle>& style) {
    m_style = style;

    if (m_borders.empty()) {
        // Generate image elements for borders.
        m_borders.reserve(NineSliceStyle::SLICE_COUNT);

        for (size_t index = 0U; index < NineSliceStyle::SLICE_COUNT; index++) {
            NineSliceStyle::Region region = static_cast<NineSliceStyle::Region>(index);

            const std::shared_ptr<Graphics::Texture2D>& p_texture = m_style->GetRegion(region);

            std::unique_ptr<UI::ImageElement> p_imageElement = std::make_unique<UI::ImageElement>();
            p_imageElement
                ->SetTexture(p_texture)
                .SetLayoutMode(LayoutMode::RELATIVE_TO_PARENT);

            m_borders.push_back(std::move(p_imageElement));
        }
    }
    else {

        for (size_t index = 0U; index < NineSliceStyle::SLICE_COUNT; index++) {

            NineSliceStyle::Region region = static_cast<NineSliceStyle::Region>(index);

            const std::shared_ptr<Graphics::Texture2D>& p_texture = m_style->GetRegion(region);

            m_borders.at(index)->SetTexture(p_texture);
        }
    }

    return *this;
}

void UI::NineSlice::CalculateSize(glm::vec2 parent_size) {

    // first calculate size of nine slice
    float borderWidth = m_style->GetBorderWidth();

    if (GetLayoutMode() == LayoutMode::RELATIVE_TO_PARENT) {

        // Calculate own size first.
        SetAbsoluteSize(parent_size * GetRelativeSize()  + GetOffsetSize());

        glm::vec2 absSize = GetAbsoluteSize();
        glm::vec2 centerSize = absSize - 2.0F * borderWidth;

        CalculateSliceSize(centerSize, borderWidth);

        // set size for each child
        for (auto& p_child : GetChildren()) {
            p_child->CalculateSize(centerSize);
        }
    }
    else if (GetLayoutMode() == LayoutMode::FIXED) {

        SetAbsoluteSize(GetFixedSize());
        glm::vec2 absSize = GetAbsoluteSize();
        // subtract border width from size.
        glm::vec2 centerSize = absSize - 2.0F * borderWidth;

        CalculateSliceSize(centerSize, borderWidth);

        for (auto& p_child : GetChildren()) {
            p_child->CalculateSize(centerSize);
        }

    }
    else if (GetLayoutMode() == LayoutMode::FIT_TO_CHILDREN) {

        glm::vec2 centerSize = parent_size - 2.0F * borderWidth;

        float sizeX = 0.0F;
        float sizeY = 0.0F;

        for (auto& p_child : GetChildren()) {

            p_child->CalculateSize(centerSize);

            glm::vec2 childSize = p_child->GetAbsoluteSize();
            sizeX = std::max(sizeX, childSize.x);
            sizeY = std::max(sizeY, childSize.y);
        }

        centerSize.x = sizeX;
        centerSize.y = sizeY;

        SetAbsoluteSize(centerSize + 2.0F*borderWidth);

        CalculateSliceSize(centerSize, borderWidth);
    }
    else {
        throw Core::EngineException("Unsupported layout option for nine slice.");
    }
}

void UI::NineSlice::CalculatePosition(glm::vec2 parent_size, glm::vec2 parent_position) {

    SetAbsolutePosition(parent_position
        + parent_size * GetRelativePosition()
        + GetOffsetPosition()
        - GetOrigin() * GetAbsoluteSize());

    glm::vec2 absSize = GetAbsoluteSize();
    glm::vec2 absPosition = GetAbsolutePosition();
    float borderSize = m_style->GetBorderWidth();
    glm::vec2 centerSize = absSize - 2.0F * borderSize;

    // calculate nine slice positions
    for (size_t index = 0U; index < m_borders.size(); index++) {
        NineSliceStyle::Region region = static_cast<NineSliceStyle::Region>(index);
        auto& p_slice = m_borders.at(index);
        glm::vec2 slicePosition = absPosition;

        switch (region) {
            case UI::NineSliceStyle::Region::TOP_LEFT_CORNER:
                break;

            case UI::NineSliceStyle::Region::TOP_RIGHT_CORNER:
                slicePosition = slicePosition + glm::vec2(borderSize + centerSize.x, 0.0F);
                break;

            case UI::NineSliceStyle::Region::BOTTOM_LEFT_CORNER:
                slicePosition = slicePosition + glm::vec2(0.0F, borderSize + centerSize.y);
                break;

            case UI::NineSliceStyle::Region::BOTTOM_RIGHT_CORNER:
                slicePosition = slicePosition + glm::vec2(borderSize) + centerSize;
                break;

            case UI::NineSliceStyle::Region::TOP_EDGE:
                slicePosition = slicePosition + glm::vec2(borderSize, 0.0F);
                break;

            case UI::NineSliceStyle::Region::BOTTOM_EDGE:
                slicePosition = slicePosition + glm::vec2(borderSize, centerSize.y + borderSize);
                break;

            case UI::NineSliceStyle::Region::LEFT_EDGE:
                slicePosition = slicePosition + glm::vec2(0.0F, borderSize);
                break;

            case UI::NineSliceStyle::Region::RIGHT_EDGE:
                slicePosition = slicePosition + glm::vec2(borderSize + centerSize.x, borderSize);
                break;

            case UI::NineSliceStyle::Region::CENTER:
                slicePosition = slicePosition + glm::vec2(borderSize);
                break;
        }

        p_slice->CalculatePosition(p_slice->GetAbsoluteSize(), slicePosition);
    }

    for (auto& p_child : GetChildren()) {
        p_child->CalculatePosition(centerSize, absPosition + glm::vec2(borderSize));
    }
}

void UI::NineSlice::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) {

    depth++;
    for (auto& slice : m_borders) {
        slice->UpdateGraphics(registry, screenSize, depth);
    }

    for (auto& child : GetChildren()) {
        child->UpdateGraphics(registry, screenSize, depth);
    }
}

void UI::NineSlice::CalculateSliceSize(glm::vec2 centerSize, float borderWidth) {
    // calculate border size
    for (size_t index = 0U; index < m_borders.size(); index++) {
        NineSliceStyle::Region region = static_cast<NineSliceStyle::Region>(index);
        auto& p_slice = m_borders.at(index);

        switch(region) {
            case UI::NineSliceStyle::Region::TOP_LEFT_CORNER:
            case UI::NineSliceStyle::Region::TOP_RIGHT_CORNER:
            case UI::NineSliceStyle::Region::BOTTOM_LEFT_CORNER:
            case UI::NineSliceStyle::Region::BOTTOM_RIGHT_CORNER:
                p_slice->CalculateSize({borderWidth, borderWidth});
                break;

            case UI::NineSliceStyle::Region::TOP_EDGE:
            case UI::NineSliceStyle::Region::BOTTOM_EDGE:
                p_slice->CalculateSize({centerSize.x, borderWidth});
                break;

            case UI::NineSliceStyle::Region::LEFT_EDGE:
            case UI::NineSliceStyle::Region::RIGHT_EDGE:
                p_slice->CalculateSize({borderWidth, centerSize.y});
                break;

            case UI::NineSliceStyle::Region::CENTER:
                p_slice->CalculateSize(centerSize);
                break;
        }
    }
}

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

//----------------------------------------------------------------------------------------------------------------------
// Button

UI::Button::Button()
    : m_current_state(ButtonState::UNKNOWN)
    , m_requested_state(ButtonState::DISABLED)
    , m_p_frame(nullptr)
    , m_p_text(nullptr) {

    SetHoverEnterCallback([this](){
        if (this->m_current_state != ButtonState::DISABLED) {
            this->SetButtonState(ButtonState::FOCUSED);
        }
    });

    SetHoverExitCallback([this]() {
        if (this->m_current_state != ButtonState::DISABLED) {
            this->SetButtonState(ButtonState::ENABLED);
        }
    });

    SetMouseButtonPressCallback([this](MouseButtonID button){

        if ((this->m_current_state != ButtonState::DISABLED) && (button == MouseButtonID::MOUSE_LEFT)) {
            this->SetButtonState(ButtonState::ACTIVATED);
        }
    });

    SetMouseButtonReleaseCallback([this](MouseButtonID button){

        Core::Logger::Info("This happend!");
        if ((this->m_current_state == ButtonState::ACTIVATED) && (button == MouseButtonID::MOUSE_LEFT)) {
            this->SetButtonState(ButtonState::FOCUSED);

            // click is registered!
            if (m_click_callback) {
                m_click_callback();
            }
        }
    });
}

UI::Button& UI::Button::SetButtonStyle(std::shared_ptr<ButtonStyle> p_style) {
    m_p_button_style = std::move(p_style);
    return *this;
}

UI::Button& UI::Button::SetText(const std::string& text) {
    m_button_text = text;
    return *this;
}

// Changes the button state to a new button state.
UI::Button& UI::Button::SetButtonState(ButtonState state) {

    if (m_current_state != state) {

        if ((m_current_state == ButtonState::UNKNOWN) && (m_p_button_style != nullptr)) {

            // need to initialize elements
            UI::NineSlice& frame = EmplaceChild<UI::NineSlice>();
            m_p_frame = &frame;

            UI::TextElement& text = frame.EmplaceChild<UI::TextElement>();
            text.SetFont(m_p_button_style->GetFont());
            text.SetText(m_p_button_style->GetFont()->CreateText(m_button_text));
            text.SetFixedSize(text.GetTextSize());
            text.SetOrigin({0.5F, 0.5F});
            text.SetLayoutMode(LayoutMode::FIXED);
            text.SetRelativePosition({0.5F, 0.5F});
            m_p_text = &text;
        }

        m_p_frame->SetStyle(m_p_button_style->GetNineSliceStyle(state));
        m_p_text->SetTextColor(m_p_button_style->GetTextColor(state));

        m_current_state = state;
    }

    return *this;
}

// Set button state change callback.
UI::Button& UI::Button::SetOnClickCallback(UI::OnClickCallback callback) {
    m_click_callback = std::move(callback);
    return *this;
}

void UI::Button::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) {

    depth++;
    for (auto& p_child : GetChildren()) {
        p_child->UpdateGraphics(registry, screenSize, depth);
    }
}
//----------------------------------------------------------------------------------------------------------------------
// Style


UI::Style UI::Style::Load(Core::Engine& engine, const std::string& filename) {

    Core::AssetLoader& assetLoader = engine.GetAssetLoader();
    Systems::TextSystem& textSystem = engine.GetEcsRegistry().GetSystem<Systems::TextSystem>();
    UI::Style style;

    std::ifstream filestream(assetLoader.GetUiDir() + "/" + filename);

    nlohmann::json styleData = nlohmann::json::parse(filestream);

    if (styleData.contains("font")) {

        for (auto& fontData : styleData["font"]) {

            std::string fontID = fontData["id"];
            float fontSize = fontData["pt"];
            bool useSDF = fontData["sdf"];
            std::string name = fontData["filename"];
            TTF_HorizontalAlignment alignment = ParseAlignment(fontData["alignment"]);

            std::shared_ptr<Graphics::Font> pFont =
                std::make_shared<Graphics::Font>(assetLoader, textSystem, name, fontSize, useSDF, alignment);

            style.SetFont(fontID, pFont);
        }
    }

    if (styleData.contains("nine-slice")) {

        for (auto& nineSliceData : styleData["nine-slice"]) {

            std::string ninesliceId = nineSliceData["id"];
            std::vector<std::string> imageFiles;
            imageFiles.reserve(NineSliceStyle::SLICE_COUNT);
            imageFiles.push_back(nineSliceData["top-left"]);
            imageFiles.push_back(nineSliceData["top-right"]);
            imageFiles.push_back(nineSliceData["bottom-left"]);
            imageFiles.push_back(nineSliceData["bottom-right"]);
            imageFiles.push_back(nineSliceData["top"]);
            imageFiles.push_back(nineSliceData["left"]);
            imageFiles.push_back(nineSliceData["right"]);
            imageFiles.push_back(nineSliceData["bottom"]);
            imageFiles.push_back(nineSliceData["center"]);

            std::shared_ptr<NineSliceStyle> nineSliceStyle = NineSliceStyle::Load(engine, imageFiles);

            style.SetNineSliceStyle(ninesliceId, std::move(nineSliceStyle));
        }
    }

    if (styleData.contains("button")) {

        for (auto& buttonData : styleData["button"]) {

            std::string buttonId = buttonData["id"];
            std::shared_ptr<ButtonStyle> p_buttonStyle = std::make_shared<ButtonStyle>();

            p_buttonStyle->SetFont(style.GetFont(buttonData["font-id"]));

            auto& disabledData = buttonData["disabled"];
            p_buttonStyle->SetNineSliceStyle(ButtonState::DISABLED, style.GetNineSliceStyle(disabledData["nine-slice-id"]));
            p_buttonStyle->SetTextColor(ButtonState::DISABLED, ParseColor(disabledData["text-color"]));

            auto& enabledData = buttonData["enabled"];
            p_buttonStyle->SetNineSliceStyle(ButtonState::ENABLED, style.GetNineSliceStyle(enabledData["nine-slice-id"]));
            p_buttonStyle->SetTextColor(ButtonState::ENABLED, ParseColor(enabledData["text-color"]));

            auto& focusData = buttonData["focused"];
            p_buttonStyle->SetNineSliceStyle(ButtonState::FOCUSED, style.GetNineSliceStyle(focusData["nine-slice-id"]));
            p_buttonStyle->SetTextColor(ButtonState::FOCUSED, ParseColor(focusData["text-color"]));

            auto& activatedData = buttonData["activated"];
            p_buttonStyle->SetNineSliceStyle(ButtonState::ACTIVATED, style.GetNineSliceStyle(activatedData["nine-slice-id"]));
            p_buttonStyle->SetTextColor(ButtonState::ACTIVATED, ParseColor(activatedData["text-color"]));

            style.SetButtonStyle(buttonId, std::move(p_buttonStyle));
        }
    }
    return style;
}

void UI::Style::SetFont(const std::string& font_id, std::shared_ptr<Graphics::Font> p_font) {
    m_fonts[font_id] = std::move(p_font);
}

std::shared_ptr<Graphics::Font>& UI::Style::GetFont(const std::string& font_id) {
    auto fontIter = m_fonts.find(font_id);
    if (fontIter == m_fonts.end()) {
        throw Core::EngineException("GetFont() failed to find font " + font_id);
    }
    return fontIter->second;
}

void UI::Style::SetNineSliceStyle(const std::string& style_id, std::shared_ptr<NineSliceStyle>&& style) {
    m_nine_slice_styles[style_id] = style;
}

std::shared_ptr<UI::NineSliceStyle>& UI::Style::GetNineSliceStyle(const std::string& nineslice_id) {
    auto styleIter = m_nine_slice_styles.find(nineslice_id);
    if (styleIter == m_nine_slice_styles.end()) {
        throw Core::EngineException("GetNineSliceStyle() failed to find style " + nineslice_id);
    }
    return styleIter->second;
}

void UI::Style::SetButtonStyle(const std::string& button_id, std::shared_ptr<ButtonStyle>&& style) {
    m_button_style[button_id] = std::move(style);
}

std::shared_ptr<UI::ButtonStyle>& UI::Style::GetButtonStyle(const std::string& button_id) {
    auto buttonIter = m_button_style.find(button_id);
    if (buttonIter == m_button_style.end()) {
        throw Core::EngineException("GetButtonStyle() failed to find style " + button_id);
    }
    return buttonIter->second;
}

TTF_HorizontalAlignment UI::Style::ParseAlignment(const std::string& asString) {
    TTF_HorizontalAlignment alignment = TTF_HORIZONTAL_ALIGN_INVALID;

    if (asString == "center") {
        alignment = TTF_HORIZONTAL_ALIGN_CENTER;
    }
    else if (asString == "left") {
        alignment = TTF_HORIZONTAL_ALIGN_LEFT;
    }
    else if (asString == "right") {
        alignment = TTF_HORIZONTAL_ALIGN_RIGHT;
    }

    return alignment;
}

glm::vec4 UI::Style::ParseColor(nlohmann::json& colorData) {

    glm::vec4 color;
    color.r = colorData["r"];
    color.g = colorData["g"];
    color.b = colorData["b"];
    color.a = colorData["a"];
    return color;
}
