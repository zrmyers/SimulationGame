#include "HorizontalLayout.hpp"


//----------------------------------------------------------------------------------------------------------------------
// Horizontal Layout Element
UI::HorizontalLayout::HorizontalLayout() {
    SetLayoutMode(LayoutMode::FIT_TO_CHILDREN);
}

void UI::HorizontalLayout::CalculateSize(glm::vec2 parent_size) {

    uint32_t num_fixed = 0U;
    uint32_t num_relative = 0U;
    // iterate through each child, processing children with fixed size layout first.
    glm::vec2 totalSize = {0.0F, -1.0F};

    if (GetLayoutMode() == LayoutMode::RELATIVE_TO_PARENT) {

        // Calculate own size first.
        SetAbsoluteSize(parent_size * GetRelativeSize()  + GetOffsetSize());
    }
    else if (GetLayoutMode() == LayoutMode::FIXED) {

        SetAbsoluteSize(GetFixedSize());
    }

    for (auto& p_child : GetChildren()) {

        if (p_child->GetLayoutMode() == LayoutMode::FIXED || p_child->GetLayoutMode() == LayoutMode::FIT_TO_CHILDREN) {
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

    if (totalSize.y < 0.0F) {
        totalSize.y = parent_size.y;
    }

    if (num_relative > 0) {

        // divide up remaining space among relative sized children.
        glm::vec2 horizontalPartitionSize
            = glm::vec2((parent_size.x - totalSize.x)/static_cast<float>(num_relative)
                , totalSize.y);
        // calculate size of children.
        for (auto& p_child : GetChildren()) {

            horizontalPartitionSize.x = std::max(horizontalPartitionSize.x, 0.0F);
            if (p_child->GetLayoutMode() == LayoutMode::RELATIVE_TO_PARENT) {
                p_child->CalculateSize(horizontalPartitionSize);
                glm::vec2 childSize = p_child->GetAbsoluteSize();
                totalSize.x += childSize.x;
                totalSize.y = std::max(childSize.y, totalSize.y);
            }
        }
    }

    if (GetLayoutMode()  == LayoutMode::FIT_TO_CHILDREN) {
        SetAbsoluteSize(totalSize);
    }

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

void UI::HorizontalLayout::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) {

    depth++;
    for (auto& p_child : GetChildren()) {
        p_child->UpdateGraphics(registry, screenSize, depth);
    }
}
