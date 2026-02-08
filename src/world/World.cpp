#include "World.hpp"
#include "Region.hpp"
#include "Tile.hpp"
#include "WorldParams.hpp"
#include <cstdint>
#include <glm/geometric.hpp>
#include <limits>

namespace World {

    World::World(const WorldParams& params)
        : m_params(params) {

        Extent_t extent = params.GetWorldExtent();
        // Initialize tiles
        size_t total_tiles = static_cast<size_t>(extent.x) * static_cast<size_t>(extent.y);
        for (size_t i = 0; i < total_tiles; ++i) {
            m_tiles.emplace_back(*this, static_cast<TileId_t>(i));
        }
    }

    const WorldParams& World::GetParameters() const {
        return m_params;
    }

    TileId_t World::CoordinateToTileId(Coordinate_t coordinate) const {

        return (coordinate.y * m_params.GetWorldExtent().x) + coordinate.x;
    }

    Coordinate_t World::TileIdToCoordinate(TileId_t tile_id) const {

        return Coordinate_t{tile_id % m_params.GetWorldExtent().x, tile_id / m_params.GetWorldExtent().x};
    }

    glm::vec2 World::CoordinateToPosition(Coordinate_t coordinate) const { // NOLINT may want to configure scale.
        glm::vec2 position(coordinate);
        position += glm::vec2(0.5F); // set position in middle of tile.
        position *= TILE_SIZE_METERS_F32;

        return position;
    }

    Coordinate_t World::PositionToCoordinate(glm::vec2 position) const { // NOLINT may want to configure scale.
        Coordinate_t coordinate;
        coordinate.x = static_cast<uint32_t>(position.x * TILE_PER_METER_F32);
        coordinate.y = static_cast<uint32_t>(position.y * TILE_PER_METER_F32);
        return coordinate;
    }

    void World::SetPlates(std::vector<TectonicPlate>&& plates) {
        m_plates = std::move(plates);
    }

    void World::SetRegions(std::vector<Region>&& regions, bool updateTiles) {
        m_regions = std::move(regions);

        if (updateTiles) {
            // Assign each tile to the nearest region centroid using Voronoi-like assignment
            for (size_t tile_idx = 0; tile_idx < m_tiles.size(); ++tile_idx) {
                Coordinate_t tile_coord = TileIdToCoordinate(static_cast<TileId_t>(tile_idx));
                glm::vec2 tile_pos = CoordinateToPosition(tile_coord);

                // Find the closest region to this tile
                RegionId_t closest_region = INVALID_REGION_ID;
                float closest_distance = std::numeric_limits<float>::max();

                for (size_t region_idx = 0; region_idx < m_regions.size(); ++region_idx) {
                    glm::vec2 region_centroid = m_regions[region_idx].GetCentroid();
                    float distance = glm::distance(tile_pos, region_centroid);

                    if (distance < closest_distance) {
                        closest_distance = distance;
                        closest_region = static_cast<RegionId_t>(region_idx);
                    }
                }

                // Assign the tile to the closest region
                m_tiles[tile_idx].SetRegionId(closest_region);
            }

            // Determine if the tile is on a region boundary.
            for (size_t tile_idx = 0; tile_idx < m_tiles.size(); tile_idx++) {

                Tile& tileA = m_tiles.at(tile_idx);
                Coordinate_t coordinate = TileIdToCoordinate(tile_idx);

                // check eastern and southern neighbors
                if (coordinate.x + 1 < m_params.GetWorldExtent().x) {
                    TileId_t tileB_idx = CoordinateToTileId({coordinate.x + 1, coordinate.y});
                    Tile& tileB = m_tiles.at(tileB_idx);

                    if (tileA.GetRegionId() != tileB.GetRegionId()) {
                        tileA.SetIsEdgeTile(true);
                        tileB.SetIsEdgeTile(true);
                    }
                }

                if (coordinate.y + 1 < m_params.GetWorldExtent().y) {
                    TileId_t tileB_idx = CoordinateToTileId({coordinate.x, coordinate.y + 1});
                    Tile& tileB = m_tiles.at(tileB_idx);

                    if (tileA.GetRegionId() != tileB.GetRegionId()) {
                        tileA.SetIsEdgeTile(true);
                        tileB.SetIsEdgeTile(true);
                    }
                }
            }
        }
    }

    void World::SetOceanLevel(float level) {
        m_ocean_level = level;
    }

    Extent_t World::GetSize() const {
        return m_params.GetWorldExtent();
    }

    Tile& World::GetTile(TileId_t tile_id) {
        return m_tiles.at(tile_id);
    }

    const Tile& World::GetTile(TileId_t tile_id) const {
        return m_tiles.at(tile_id);
    }

    Region& World::GetRegion(RegionId_t region_id) {
        return m_regions.at(region_id);
    }

    const Region& World::GetRegion(RegionId_t region_id) const {
        return m_regions.at(region_id);
    }

    TectonicPlate& World::GetPlate(PlateId_t plate_id) {
        return m_plates.at(plate_id);
    }

    const TectonicPlate& World::GetPlate(PlateId_t plate_id) const {
        return m_plates.at(plate_id);
    }

    const std::vector<Tile>& World::GetTiles() const {
        return m_tiles;
    }

    std::vector<Tile>& World::GetTiles() {
        return m_tiles;
    }

    const std::vector<Region>& World::GetRegions() const {
        return m_regions;
    }

    std::vector<Region>& World::GetRegions() {
        return m_regions;
    }

    const std::vector<TectonicPlate>& World::GetPlates() const {
        return m_plates;
    }

    std::vector<TectonicPlate>& World::GetPlates() {
        return m_plates;
    }

    float World::GetOceanLevel() const {
        return m_ocean_level;
    }

}
