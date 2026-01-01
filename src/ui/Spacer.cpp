/**
 * @file Spacer.cpp
 * @brief Implementation of the `UI::Spacer` layout helper.
 */

#include "Spacer.hpp"

//----------------------------------------------------------------------------------------------------------------------
// Spacer

UI::Spacer::Spacer() {
    SetLayoutMode(LayoutMode::RELATIVE_TO_PARENT);
}

void UI::Spacer::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) {
    for (auto& p_child : GetChildren()) {
        p_child->UpdateGraphics(registry, screenSize, depth);
    }
}
