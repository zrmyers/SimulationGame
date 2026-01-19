#pragma once

#include <glm/ext/vector_uint2.hpp>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <vector>

#include "Region.hpp"
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
            TileId_t CoordinateToTileId(Coordinate_t coordinate) const;

            //! @brief Convert a tile ID to a world coordinate.
            //!
            //! @param[in] tile_id The ID of tile to get coordinate of.
            //!
            //! @returns The coordinate of the tile.
            Coordinate_t TileIdToCoordinate(TileId_t tile_id) const;

            //! @brief Convert a coordinate to a position.
            glm::vec2 CoordinateToPosition(Coordinate_t coordinate) const;

            //! @brief Convert a world position to a coordinate.
            Coordinate_t PositionToCoordinate(glm::vec2 position) const;

            //! Set plates
            void SetPlates(std::vector<TectonicPlate>&& plates);
            void SetRegions(std::vector<Region>&& regions);

            //! Get the size of the world
            Extent_t GetSize() const;

            //! Get a tile by ID
            Tile& GetTile(TileId_t tile_id);
            const Tile& GetTile(TileId_t tile_id) const;

            //! Get a region by ID
            Region& GetRegion(RegionId_t region_id);
            const Region& GetRegion(RegionId_t region_id) const;

            //! Get a plate by ID
            TectonicPlate& GetPlate(PlateId_t plate_id);
            const TectonicPlate& GetPlate(PlateId_t plate_id) const;

            //! Get all tiles
            const std::vector<Tile>& GetTiles() const;
            std::vector<Tile>& GetTiles();

            //! Get all regions
            const std::vector<Region>& GetRegions() const;
            std::vector<Region>& GetRegions();

            //! Get all plates
            const std::vector<TectonicPlate>& GetPlates() const;
            std::vector<TectonicPlate>& GetPlates();

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