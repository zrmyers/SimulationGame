#pragma once

#include "glm/mat4x4.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>

namespace Components {

    struct Transform {
        glm::mat4x4 m_transform{1.0F};

        Transform& Set(const glm::mat4x4& other) {
            m_transform = other;
            return *this;
        }

        Transform& Translate(glm::vec3 translate) {
            m_transform = glm::translate(m_transform, translate);
            return *this;
        }

        Transform& Scale(glm::vec3 scale) {
            m_transform = glm::scale(m_transform, scale);
            return *this;
        }

        Transform& Rotate(float angle, glm::vec3 axis) {
            m_transform = glm::rotate(m_transform, angle, axis);
            return *this;
        }
    };
}