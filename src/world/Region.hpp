#pragma once

#include "TectonicPlate.hpp"
#include <cstdint>

namespace World {

    class World;

    using RegionId_t = int32_t;

    static constexpr RegionId_t INVALID_REGION_ID = -1;

    // A group of tiles that form a fundamental unit of land in the game. Determines biome and ownership for a range of
    // tiles.
    class Region {

        public:
            Region(World& world, glm::vec2 centroid, std::vector<RegionId_t>&& neighbors);

            void SetPlateId(PlateId_t plate_id);
            PlateId_t GetPlateId() const;

            glm::vec2 GetCentroid() const;

            //! Get the neighbors of this region
            const std::vector<RegionId_t>& GetNeighbors() const;

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