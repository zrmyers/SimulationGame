#include "Element.hpp"
#include <glm/common.hpp>

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
    else if (m_layout_mode == LayoutMode::FIT_TO_CHILDREN) {

        glm::vec2 childMaxSize = {0.0F, 0.0F};
        for (auto& p_child : m_children) {
            p_child->CalculateSize(parent_size);

            childMaxSize = glm::max(childMaxSize, p_child->GetAbsoluteSize());
        }

        SetAbsoluteSize(childMaxSize);
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

        if (m_hover_enter_callback && currentCollision) {
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

void UI::Element::ClearGraphics() {

    for (auto& p_child : m_children) {
        p_child->ClearGraphics();
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
    child->m_p_parent = this;
    m_children.emplace_back(std::move(child));
}

std::vector<std::unique_ptr<UI::Element>>& UI::Element::GetChildren() {
    return m_children;
}

size_t UI::Element::GetChildCount() {
    return m_children.size();
}

UI::Element& UI::Element::GetChild(size_t child_index) {
    return *m_children.at(child_index);
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

void UI::Element::SetDirty() {

    m_is_dirty = true;

    if (m_p_parent != nullptr) {
        m_p_parent->SetDirty();
    }
}

void UI::Element::ClearDirty() {

    m_is_dirty = false;

    for (auto& p_child : m_children) {

        if (p_child->GetDirty()) {
            p_child->ClearDirty();
        }
    }
}

// Get whether the given element is dirty.
bool UI::Element::GetDirty() const {

    return m_is_dirty;
}