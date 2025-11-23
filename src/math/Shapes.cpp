#include "Shapes.hpp"

#include "glm/common.hpp"

Math::Box::Box(glm::vec2 top_left, glm::vec2 extent)
    : m_extent {extent}
    , m_topLeft {top_left} {
}

glm::vec2 Math::Box::GetTopLeft() const {
    return m_topLeft;
}

glm::vec2 Math::Box::GetTopRight() const {

    glm::vec2 topRight = m_topLeft;
    topRight.x += m_extent.x; // NOLINT
    return topRight;
}

glm::vec2 Math::Box::GetBottomLeft() const {
    glm::vec2 bottomLeft = m_topLeft;
    bottomLeft.y += m_extent.y;
    return bottomLeft;
}

glm::vec2 Math::Box::GetBottomRight() const {
    glm::vec2 bottomRight = m_topLeft + m_extent;
    return bottomRight;
}

glm::vec2 Math::Box::GetExtent() const {
    return m_extent;
}
glm::vec2 Math::Box::GetCenter() const {
    return m_topLeft + m_extent/2.0f; // NOLINT
}

float Math::Box::GetWidth() const {
    return m_extent.x;
}

float Math::Box::GetHeight() const {
    return m_extent.y;
}

float Math::Box::GetTop() const {
    return m_topLeft.y;
}

float Math::Box::GetLeft() const {
    return m_topLeft.x;
}

float Math::Box::GetBottom() const {
    return m_topLeft.y + m_extent.y;
}

float Math::Box::GetRight() const {
    return m_topLeft.x + m_extent.x;
}

void Math::Box::SetTopLeft(const glm::vec2& position) {
    m_topLeft = position;
}

void Math::Box::Resize(float width, float height) {
    m_extent.x = width;
    m_extent.y = height;
}

void Math::Box::Combine(const Box& other) {

    m_topLeft = glm::min(other.GetTopLeft(), GetTopLeft());
    m_extent = glm::max(other.GetBottomRight(), GetBottomRight());
}

void Math::Box::Center(const glm::vec2& coord) {

    m_topLeft = coord - m_extent/2.0F;
}

bool Math::Box::CheckIsAdjacent(const Box& other) const {
    bool isAdjacent = false;

    if ((GetTopLeft() == other.GetBottomLeft()) && (GetTopRight() == other.GetBottomRight())) {

        // other is above
        isAdjacent = true;
    }
    else if ((GetTopLeft() == other.GetTopRight()) && (GetBottomLeft() == other.GetBottomRight())) {

        // other is to left
        isAdjacent = true;
    }
    else if ((GetBottomRight() == other.GetTopRight()) && (GetBottomLeft() == other.GetTopLeft())) {

        // other is below.
        isAdjacent = true;
    }
    else if ((GetBottomRight() == other.GetBottomLeft() && (GetTopRight() == other.GetTopLeft()))) {

        // other is to right.
        isAdjacent = true;
    }

    return isAdjacent;
}

bool Math::Box::CheckCollision(const glm::vec2& coord) const {
    bool isCollision = false;
    glm::vec2 delta =  coord - m_topLeft;

    if ((delta.x >= 0.0F)
        && (delta.y >= 0.0F)
        && (delta.x <= m_extent.x)
        && (delta.y <= m_extent.y)) {
        isCollision = true;
    }

    return isCollision;
}

bool Math::Box::operator==(const Box& other) const {

    return ((m_extent == other.GetExtent()) && (m_topLeft == other.GetTopLeft()));
}