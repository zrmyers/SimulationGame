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

        private:

            World* m_p_world {nullptr};
            TileId_t m_tile_id {INVALID_TILE_ID};
            RegionId_t m_region_id {INVALID_REGION_ID};
    };
}