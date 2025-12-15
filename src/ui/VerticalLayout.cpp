#include "VerticalLayout.hpp"


//----------------------------------------------------------------------------------------------------------------------
// Vertical Layout Element
UI::VerticalLayout::VerticalLayout() {
    SetLayoutMode(LayoutMode::FIT_TO_CHILDREN);
}

void UI::VerticalLayout::CalculateSize(glm::vec2 parent_size) {

    uint32_t num_fixed = 0U;
    uint32_t num_relative = 0U;
    // iterate through each child, processing children with fixed size layout first.
    glm::vec2 totalSize = {-1.0F, 0.0F};

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
            totalSize.y += childFixedSize.y;
            totalSize.x = std::max(totalSize.x, childFixedSize.x);
            num_fixed++;
        }
        else {
            num_relative++;
        }
    }

    if (totalSize.x < 0.0F) {
        totalSize.x = parent_size.x;
    }

    if (num_relative > 0) {

        // divide up remaining space among relative sized children.
        glm::vec2 verticalPartitionSize
            = glm::vec2(totalSize.x,
             (parent_size.y - totalSize.y)/static_cast<float>(num_relative));
        // calculate size of children.
        for (auto& p_child : GetChildren()) {

            verticalPartitionSize.y = std::max(verticalPartitionSize.y, 0.0F);
            if (p_child->GetLayoutMode() == LayoutMode::RELATIVE_TO_PARENT) {
                p_child->CalculateSize(verticalPartitionSize);
                glm::vec2 childSize = p_child->GetAbsoluteSize();
                totalSize.y += childSize.y;
                totalSize.x = std::max(childSize.x, totalSize.x);
            }
        }
    }

    if (GetLayoutMode() == LayoutMode::FIT_TO_CHILDREN) {
        SetAbsoluteSize(totalSize);
    }
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

void UI::VerticalLayout::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) {
    depth++;
    for (auto& p_child : GetChildren()) {
        p_child->UpdateGraphics(registry, screenSize, depth);
    }
}
