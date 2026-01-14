#pragma once

#include <cstddef>
#include <cstdint>
#include <glm/ext/vector_uint2.hpp>
#include <glm/vec2.hpp>
#include <vector>

namespace World {

    using Extent_t = glm::uvec2;
    using Coordinate_t = glm::uvec2;
    using TileId_t = size_t;

    static constexpr TileId_t INVALID_TILE_ID = SIZE_MAX;

    class World;

    // 2D Grid Used to represent a location in the world.
    class Tile {

        public:
            Tile() = default;
            Tile(TileId_t tile_id);

        private:

            TileId_t m_tile_id {INVALID_TILE_ID};
    };

    class World {

        public:

            World(Extent_t size);

            //! @brief Convert a world coordinate to a tile ID.
            //!
            //! @param[in] coordinate The coordinate to convert
            //!
            //! @returns The ID of the tile at the coordinate.
            TileId_t CoordinateToTileId(Coordinate_t coordinate);

            Coordinate_t TileIdToCoordinate(TileId_t tile_id);

        private:

            //! Set of all tiles in the world
            std::vector<Tile> m_tiles;

            //! The x-y dimmension of the world.
            Extent_t m_size;
    };
}