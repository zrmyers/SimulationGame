#pragma once

#include "glm/vec2.hpp"
#include <vector>

namespace World {

    class World;

    using PlateId_t = uint32_t;

    static constexpr PlateId_t INVALID_PLATE_ID = UINT32_MAX;

    // Represents
    class TectonicPlate {

        public:

            TectonicPlate(World& world, glm::vec2 centroid, std::vector<PlateId_t>&& neighbors);

            void SetVelocity(glm::vec2 velocity);
            glm::vec2 GetVelocity() const;

            void SetIsContinental(bool continental);
            bool GetIsContinental() const;

        private:

            //! Reference to world that region belongs to.
            World* m_p_world;

            //! The point that represents the center of the region.
            glm::vec2 m_centroid;

            //! The list of neighbors for the region.
            std::vector<PlateId_t> m_neighbors;

            //! The velocity of the plate. Used for determining plate boundary types.
            glm::vec2 m_velocity;

            //! Whether the plate is continental or oceanic. If true the plate is continental, if false the plate is
            //! oceanic.
            bool m_is_continental;
    };
}