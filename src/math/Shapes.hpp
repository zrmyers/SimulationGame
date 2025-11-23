#pragma once

#include <glm/ext/vector_float2.hpp>
namespace Math {

    class Box {

        public:
            Box() = default;
            Box(glm::vec2 top_left, glm::vec2 extent);

            glm::vec2 GetTopLeft() const;
            glm::vec2 GetTopRight() const;
            glm::vec2 GetBottomLeft() const;
            glm::vec2 GetBottomRight() const;

            glm::vec2 GetExtent() const;
            glm::vec2 GetCenter() const;

            float GetWidth() const;
            float GetHeight() const;
            float GetTop() const;
            float GetLeft() const;
            float GetBottom() const;
            float GetRight() const;

            void SetTopLeft(const glm::vec2& position);
            void Resize(float width, float height);
            void Combine(const Box& other);
            void Center(const glm::vec2& coord);

            bool CheckIsAdjacent(const Box& other) const;
            bool CheckCollision(const glm::vec2& coord) const;

            bool operator==(const Box& other) const;

        private:

            glm::vec2 m_extent;
            glm::vec2 m_topLeft;
    };
}