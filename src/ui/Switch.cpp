#include "Switch.hpp"
#include "Element.hpp"
#include <cstddef>


UI::Switch::Switch()
    : m_selected_child(0U) {
}

void UI::Switch::SelectChild(size_t index) {

    m_selected_child = index;

    // clear graphics for all children
    for (auto& p_child : GetChildren()) {
        p_child->ClearGraphics();
    }
}

void UI::Switch::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) {

    if (m_selected_child < GetChildCount()) {

        UI::Element& element = GetChild(m_selected_child);

        element.UpdateGraphics(registry, screenSize, depth);
    }
}
