#pragma once

#include "Region.hpp"

namespace World {

    using TileId_t = uint32_t;
    static constexpr TileId_t INVALID_TILE_ID = UINT32_MAX;
    class World;

    // 2D Grid Used to represent a location in the world.
    class Tile {

        public:
            Tile(World& world, TileId_t tile_id);

            void SetRegionId(RegionId_t region_id);
            RegionId_t GetRegionId() const;

            TileId_t GetTileId() const;

            glm::vec2 GetCenter() const;

            // Determine if the tile is on a the edge of a region.
            void SetIsEdgeTile(bool is_edge);
            bool GetIsEdgeTile() const;

            // Set the absolute height of the tile
            void SetAbsoluteHeight(float height);
            float GetAbsoluteHeight() const;

        private:

            World* m_p_world {nullptr};
            TileId_t m_tile_id {INVALID_TILE_ID};
            RegionId_t m_region_id {INVALID_REGION_ID};
            glm::vec2 m_center {0.0F, 0.0F};
            float m_height {0.0F};
            bool m_is_edge { false };
    };
}