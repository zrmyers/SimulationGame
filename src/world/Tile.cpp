#include "Tile.hpp"
#include "World.hpp"
#include <glm/geometric.hpp>

namespace World {

    Tile::Tile(World& world, TileId_t tile_id)
        : m_p_world(&world), m_tile_id(tile_id), m_region_id(INVALID_REGION_ID) {
        // Calculate the center position of this tile
        Coordinate_t coordinate = world.TileIdToCoordinate(tile_id);
        m_center = world.CoordinateToPosition(coordinate);
    }

    void Tile::SetRegionId(RegionId_t region_id) {
        m_region_id = region_id;
    }

    RegionId_t Tile::GetRegionId() const {
        return m_region_id;
    }

    TileId_t Tile::GetTileId() const {
        return m_tile_id;
    }

    glm::vec2 Tile::GetCenter() const {
        return m_center;
    }

}
