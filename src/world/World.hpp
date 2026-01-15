#pragma once

#include <glm/ext/vector_uint2.hpp>
#include <glm/vec2.hpp>
#include <vector>

#include "TectonicPlate.hpp"
#include "Tile.hpp"

namespace World {

    using Extent_t = glm::uvec2;
    using Coordinate_t = glm::uvec2;

    class World {

        public:

            World(Extent_t size);

            //! @brief Convert a world coordinate to a tile ID.
            //!
            //! @param[in] coordinate The coordinate to convert
            //!
            //! @returns The ID of the tile at the coordinate.
            TileId_t CoordinateToTileId(Coordinate_t coordinate);

            //! @brief Convert a tile ID to a world coordinate.
            //!
            //! @param[in] tile_id The ID of tile to get coordinate of.
            //!
            //! @returns The coordinate of the tile.
            Coordinate_t TileIdToCoordinate(TileId_t tile_id);

            void SetPlates(std::vector<TectonicPlate>&& plates);
            void SetRegions(std::vector<Region>&& regions);

        private:

            //! The x-y dimmension of the world.
            Extent_t m_size;

            //! Set of all tiles in the world
            std::vector<Tile> m_tiles;

            //! Set of all regions in the world
            std::vector<Region> m_regions;

            //! Set of all tectonic plates in the world.
            std::vector<TectonicPlate> m_plates;
    };
}