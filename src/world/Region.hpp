#pragma once

#include "TectonicPlate.hpp"
#include <cstdint>
#include <functional>

namespace World {

    class World;
    class Region;

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

            void SetIsBoundary(bool is_boundary);
            bool GetIsBoundary() const;

            std::pair<PlateBoundaryType, PlateId_t> GetPlateBoundaryType() const;

            // check for subduction
            void SetHasSubduction(bool has_subduction);
            bool GetHasSubduction() const;

            // Set the absolute height, relative to magma level.
            void SetAbsoluteHeight(float height);
            float GetAbsoluteHeight() const;

        private:

            //! Reference to world that region belongs to.
            World* m_p_world {nullptr};

            //! The point that represents the center of the region.
            glm::vec2 m_centroid {0.0F, 0.0F};

            //! The list of neighbors for the region.
            std::vector<RegionId_t> m_neighbors;

            //! The ID of the plate associated with region.
            PlateId_t m_plate_id {INVALID_PLATE_ID};

            //! The absolute height of the region
            float m_height {0.0F};

            //! Whether the region is at plate boundary.
            bool m_is_boundary {false};

            //! Whether the region is part of subduction zone.
            bool m_has_subduction {false};
    };
}