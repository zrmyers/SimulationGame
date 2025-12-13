#include "NineSlice.hpp"

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
