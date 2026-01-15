#pragma once

#include "TectonicPlate.hpp"

namespace World {

    class World;

    using RegionId_t = uint32_t;

    static constexpr RegionId_t INVALID_REGION_ID = UINT32_MAX;

    // A group of tiles that form a fundamental unit of land in the game. Determines biome and ownership for a range of
    // tiles.
    class Region {

        public:
            Region(World& world, glm::vec2 centroid, std::vector<RegionId_t>&& neighbors);

            void SetPlateId(PlateId_t plate_id);
            PlateId_t GetPlateId() const;

        private:

            //! Reference to world that region belongs to.
            World* m_p_world;

            //! The point that represents the center of the region.
            glm::vec2 m_centroid;

            //! The list of neighbors for the region.
            std::vector<RegionId_t> m_neighbors;

            //! The ID of the plate associated with region.
            PlateId_t m_plate_id;
    };
}