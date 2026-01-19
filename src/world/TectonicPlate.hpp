#pragma once

#include "glm/vec2.hpp"
#include <unordered_map>
#include <vector>

namespace World {

    class World;

    using PlateId_t = int32_t;

    static constexpr PlateId_t INVALID_PLATE_ID = -1;

    //! type of boundary. Listed in order from lowest to highest priority for defining region topology.
    enum class PlateBoundaryType : uint8_t {

        //! No boundary.
        NONE = 0,

        //! plates are sliding against each other.
        TRANSFORM,

        //! plates are away from each other.
        DIVERGENT,

        //! plates are moving toward each other.
        CONVERGENT,
    };

    // Represents
    class TectonicPlate {

        public:

            TectonicPlate(World& world, glm::vec2 centroid);

            void SetVelocity(glm::vec2 velocity);
            glm::vec2 GetVelocity() const;

            void SetIsContinental(bool continental);
            bool GetIsContinental() const;

            bool HasBoundary(PlateId_t other) const;
            void AddBoundary(PlateId_t other, PlateBoundaryType boundary_type);
            PlateBoundaryType GetBoundaryType(PlateId_t plate_id) const;

            //! Get the centroid of this plate
            glm::vec2 GetCentroid() const;

            //! Set the height of the plate. This is height relative to surface of mantle.
            void SetAbsoluteHeight(float height);
            float GetAbsoluteHeight() const;

        private:

            //! Reference to world that region belongs to.
            World* m_p_world {nullptr};

            //! The point that represents the center of the region.
            glm::vec2 m_centroid {0.0F, 0.0F};

            //! The velocity of the plate. Used for determining plate boundary types.
            glm::vec2 m_velocity {0.0F, 0.0F};

            //! The height of the plate.
            float m_height;

            //! Whether the plate is continental or oceanic. If true the plate is continental, if false the plate is
            //! oceanic.
            bool m_is_continental {false};

            //! Boundaries with neighbors
            std::unordered_map<PlateId_t, PlateBoundaryType> m_boundaries;
    };

    //! Determine the type of boundary between two tectonic plates
    //!
    //! @param[in] plate1 The first tectonic plate
    //! @param[in] plate2 The second tectonic plate
    //! @param[in] divergence_threshold The threshold angle (in degrees) to classify as divergent.
    //!                                   If the relative velocity component along the boundary normal
    //!                                   exceeds this threshold, the boundary is divergent.
    //!
    //! @returns The type of plate boundary between the two plates
    PlateBoundaryType DeterminePlateBoundaryType(const TectonicPlate& plate1, const TectonicPlate& plate2,
                                                  float divergence_threshold = 30.0F);
}