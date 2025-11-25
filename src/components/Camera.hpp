#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/trigonometric.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"

namespace Components {

    struct Camera {
            static constexpr glm::vec3 DEFAULT_DIR = {0.0F, 1.0F, 0.0F};
            static constexpr glm::vec3 DEFAULT_POS = {1.0F, 1.0F, 1.0F };
            static constexpr glm::vec3 DEFAULT_UP = {0.0F, 0.0F, 1.0F};
            static constexpr float DEFAULT_FOV_RAD = glm::pi<float>() / 2.0F;
            static constexpr float DEFAULT_ASPECT_RATIO = 1024.0F / 768.0F;
            static constexpr float DEFAULT_NEAR_CLIP = 0.1F;
            static constexpr float DEFAULT_FAR_CLIP = 100.0F;

            glm::vec3 m_direction {DEFAULT_DIR}; // direction of camera
            glm::vec3 m_position {DEFAULT_POS}; // position of the camera.
            glm::vec3 m_up {DEFAULT_UP}; // direction of 'up'

            float m_fov_radians {DEFAULT_FOV_RAD}; // field of view
            float m_aspect_ratio {DEFAULT_ASPECT_RATIO};
            float m_near_clip {DEFAULT_NEAR_CLIP};
            float m_far_clip {DEFAULT_FAR_CLIP};

            // Transformations
            Camera& Rotate(float delta, const glm::vec3& axis) {
                m_direction = glm::rotate(m_direction, delta, axis);
                return *this;
            }

            Camera& Translate(const glm::vec3& direction) {
                m_position += direction;
                return *this;
            }

            // Calculated
            glm::vec3 GetForward() const {
                return m_direction;
            }

            glm::vec3 GetBackward() const {
                return -m_direction;
            }

            glm::vec3 GetLeft() const {
                return glm::cross(-m_direction, m_up);
            }
            glm::vec3 GetRight() const {
                return glm::cross(m_direction, m_up);
            }

            glm::mat4 GetProjection() const {
                return glm::perspective(m_fov_radians, m_aspect_ratio, m_near_clip, m_far_clip);
            }

            glm::mat4 GetView() const {
                return glm::lookAt(m_position, m_position + m_direction, m_up);
            }
    };
}