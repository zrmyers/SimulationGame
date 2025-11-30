#include "UI.hpp"
#include <algorithm>
#include <memory>
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

void UI::Element::CalculateSize(glm::vec2 parent_size) {

    SetAbsoluteSize(parent_size * GetRelativeSize()  + GetOffsetSize());
}

void UI::Element::CalculatePosition(glm::vec2 parent_size, glm::vec2 parent_position) {

    // use relative position to calculate own position.
    SetAbsolutePosition(parent_position
        + parent_size * GetRelativePosition()
        + GetOffsetPosition()
        - GetOrigin() * GetAbsoluteSize());
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
// Image

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