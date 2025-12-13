#include "Switch.hpp"
#include "Element.hpp"


UI::Switch::Switch()
    : m_selected_child(0U) {
    SetLayoutMode(LayoutMode::FIT_TO_CHILDREN);
}

void UI::Switch::SelectChild(size_t index) {

    m_selected_child = index;
}

void UI::Switch::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) {

    if (m_selected_child < GetChildCount()) {

        UI::Element& element = GetChild(m_selected_child);

        element.UpdateGraphics(registry, screenSize, depth);
    }
}
