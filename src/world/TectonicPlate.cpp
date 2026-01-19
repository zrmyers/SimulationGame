#include "TectonicPlate.hpp"
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

namespace World {

    TectonicPlate::TectonicPlate(World& world, glm::vec2 centroid)
        : m_p_world(&world), m_centroid(centroid) {
    }

    void TectonicPlate::SetVelocity(glm::vec2 velocity) {
        m_velocity = velocity;
    }

    glm::vec2 TectonicPlate::GetVelocity() const {
        return m_velocity;
    }

    void TectonicPlate::SetIsContinental(bool continental) {
        m_is_continental = continental;
    }

    bool TectonicPlate::GetIsContinental() const {
        return m_is_continental;
    }

    bool TectonicPlate::HasBoundary(PlateId_t other) const {
        return m_boundaries.find(other) != m_boundaries.end();
    }

    void TectonicPlate::AddBoundary(PlateId_t other, PlateBoundaryType boundary_type) {
        m_boundaries[other] = boundary_type;
    }

    PlateBoundaryType TectonicPlate::GetBoundaryType(PlateId_t plate_id) const {
        // Find the boundary
        auto boundaryIter = m_boundaries.find(plate_id);
        if (boundaryIter != m_boundaries.end()) {
            return boundaryIter->second;
        }
        // Default to divergent if no boundaries exist
        return PlateBoundaryType::DIVERGENT;
    }

    glm::vec2 TectonicPlate::GetCentroid() const {
        return m_centroid;
    }

    void TectonicPlate::SetAbsoluteHeight(float height) {
        m_height = height;
    }

    float TectonicPlate::GetAbsoluteHeight() const {
        return m_height;
    }

    PlateBoundaryType DeterminePlateBoundaryType(const TectonicPlate& plate1, const TectonicPlate& plate2,
                                                  float divergence_threshold) {
        // Get the positions and velocities
        glm::vec2 centroid1 = plate1.GetCentroid();
        glm::vec2 centroid2 = plate2.GetCentroid();
        glm::vec2 velocity1 = plate1.GetVelocity();
        glm::vec2 velocity2 = plate2.GetVelocity();

        // Relative velocity of plate2 with respect to plate1
        glm::vec2 relative_velocity = velocity2 - velocity1;

        // If plates are stationary, classify as transform
        if (glm::length(relative_velocity) < 0.0001F) {
            return PlateBoundaryType::TRANSFORM;
        }
        // Vector from plate1 to plate2. direction plate1 has to move in order to reach plate 2.
        glm::vec2 boundary_vector = glm::normalize(centroid2 - centroid1);

        // Normalize relative velocity. direction plate1 appears to be moving relative to plate 2
        glm::vec2 normalized_relative_velocity = glm::normalize(relative_velocity);

        // Calculate the component of relative velocity along the boundary vector
        float convergence_component = glm::dot(normalized_relative_velocity, boundary_vector);

        // Convert threshold from degrees to determine convergence angle
        float threshold_ratio = glm::cos(glm::radians(divergence_threshold));

        // Classify based on convergence component
        if (convergence_component > threshold_ratio) {
            // Plates moving toward each other
            return PlateBoundaryType::CONVERGENT;
        } else if (convergence_component < -threshold_ratio) {
            // Plates moving away from each other
            return PlateBoundaryType::DIVERGENT;
        } else {
            // Plates sliding past each other (transform)
            return PlateBoundaryType::TRANSFORM;
        }
    }

}
