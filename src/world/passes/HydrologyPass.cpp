#include "Passes.hpp"

#include "world/Tile.hpp"
#include "world/Region.hpp"
#include "world/World.hpp"
#include <algorithm>
#include <glm/geometric.hpp>
#include <limits>

namespace World::Passes {

//! Calculate ocean level by finding the height threshold that matches desired land percentage
float CalculateOceanLevel(const std::vector<Tile>& tiles, float percent_land) {
    if (tiles.empty()) {
        return 0.0F;
    }

    // Collect all unique elevations and sort them
    std::vector<float> elevations;
    elevations.reserve(tiles.size());

    for (const auto& tile : tiles) {
        elevations.push_back(tile.GetAbsoluteHeight());
    }

    std::sort(elevations.begin(), elevations.end());

    // Calculate target number of land tiles
    float target_land_tiles = static_cast<float>(tiles.size()) * (percent_land * 0.01F);

    // Find the elevation threshold that gives us approximately the desired land distribution
    // We want to find the height where roughly (100 - percent_land)% of tiles are below it (ocean)
    size_t target_ocean_index = static_cast<size_t>(static_cast<float>(tiles.size()) * (1.0F - (percent_land * 0.01F)));
    target_ocean_index = glm::min(target_ocean_index, elevations.size() - 1);

    // Return the elevation at the target index
    return elevations[target_ocean_index];
}

//! Mark all tiles below ocean level as water
void MarkOceanTiles(std::vector<Tile>& tiles, float ocean_level) {
    for (auto& tile : tiles) {
        if (tile.GetAbsoluteHeight() < ocean_level) {
            tile.SetIsWater(true);
            tile.SetWaterLevel(ocean_level);
        }
    }
}

//! Initialize flow accumulation for all non-water tiles
void InitializeFlowAccumulation(std::vector<Tile>& tiles) {
    for (auto& tile : tiles) {
        if (!tile.GetIsWater()) {
            tile.SetFlowAccumulation(1.0F);
        }
    }
}

//! Create a sorted list of tile IDs ordered by elevation (highest to lowest)
std::vector<TileId_t> CreateSortedTilesByElevation(const std::vector<Tile>& tiles) {
    std::vector<TileId_t> sorted_ids;
    sorted_ids.reserve(tiles.size());

    for (size_t i = 0; i < tiles.size(); ++i) {
        sorted_ids.push_back(static_cast<TileId_t>(i));
    }

    std::sort(sorted_ids.begin(), sorted_ids.end(),
        [&tiles](TileId_t a, TileId_t b) {
            return tiles[a].GetAbsoluteHeight() > tiles[b].GetAbsoluteHeight();
        });

    return sorted_ids;
}

//! Get the elevation of a tile, accounting for water levels
float GetTileElevation(const Tile& tile) {
    return tile.GetIsWater() ? tile.GetWaterLevel() : tile.GetAbsoluteHeight();
}

//! Find the lowest neighbor and its elevation
//! Returns true if a lower neighbor was found
bool FindLowestNeighbor(const World& world, Coordinate_t coord, float current_elevation,
                        Coordinate_t& out_lowest_neighbor, float& out_lowest_elevation) {
    const Extent_t size = world.GetSize();
    const auto& tiles = world.GetTiles();

    out_lowest_neighbor = coord;
    out_lowest_elevation = current_elevation;
    bool found_lower = false;

    Coordinate_t neighbors[4] = {
        {coord.x + 1, coord.y},
        {coord.x - 1, coord.y},
        {coord.x, coord.y + 1},
        {coord.x, coord.y - 1}
    };

    for (const auto& neighbor_coord : neighbors) {
        if (neighbor_coord.x >= size.x || neighbor_coord.y >= size.y) {
            continue;
        }

        TileId_t neighbor_id = world.CoordinateToTileId(neighbor_coord);
        const Tile& neighbor = tiles[neighbor_id];
        float neighbor_elevation = GetTileElevation(neighbor);

        if (neighbor_elevation < out_lowest_elevation) {
            out_lowest_elevation = neighbor_elevation;
            out_lowest_neighbor = neighbor_coord;
            found_lower = true;
        }
    }

    return found_lower;
}

//! Trace water flow and accumulate flow volumes
void TraceWaterFlow(World& world, const std::vector<TileId_t>& sorted_tile_ids) {
    auto& tiles = world.GetTiles();

    for (TileId_t tile_id : sorted_tile_ids) {
        Tile& tile = tiles[tile_id];

        if (tile.GetIsWater()) {
            continue;  // Water tiles don't flow further
        }

        Coordinate_t coord = world.TileIdToCoordinate(tile_id);
        float tile_elevation = tile.GetAbsoluteHeight();

        Coordinate_t lowest_neighbor;
        float lowest_elevation;

        if (FindLowestNeighbor(world, coord, tile_elevation, lowest_neighbor, lowest_elevation)) {
            tile.SetFlowDirection(lowest_neighbor);

            // Accumulate flow to the lowest neighbor
            TileId_t lowest_neighbor_id = world.CoordinateToTileId(lowest_neighbor);
            Tile& lowest_neighbor_tile = tiles[lowest_neighbor_id];
            float accumulated = lowest_neighbor_tile.GetFlowAccumulation();
            lowest_neighbor_tile.SetFlowAccumulation(accumulated + tile.GetFlowAccumulation());
        }
    }
}

//! Check if a tile is a local minimum (all neighbors are higher or equal elevation)
bool IsLocalMinimum(const World& world, const Tile& tile, Coordinate_t coord) {
    const Extent_t size = world.GetSize();
    const auto& tiles = world.GetTiles();
    float tile_elevation = GetTileElevation(tile);

    Coordinate_t neighbors[4] = {
        {coord.x + 1, coord.y},
        {coord.x - 1, coord.y},
        {coord.x, coord.y + 1},
        {coord.x, coord.y - 1}
    };

    for (const auto& neighbor_coord : neighbors) {
        if (neighbor_coord.x >= size.x || neighbor_coord.y >= size.y) {
            continue;
        }

        TileId_t neighbor_id = world.CoordinateToTileId(neighbor_coord);
        const Tile& neighbor = tiles[neighbor_id];
        float neighbor_elevation = GetTileElevation(neighbor);

        if (neighbor_elevation <= tile_elevation) {
            return false;
        }
    }

    return true;
}

//! Find the lowest overflow point for a lake (lowest adjacent tile that's not part of the lake)
bool FindLakeOverflowPoint(const World& world, const Tile& lake_tile, Coordinate_t lake_coord,
                           Coordinate_t& out_overflow_coord, float& out_overflow_elevation) {
    const Extent_t size = world.GetSize();
    const auto& tiles = world.GetTiles();

    float lowest_overflow_elevation = std::numeric_limits<float>::max();
    Coordinate_t lowest_overflow = lake_coord;
    bool found_overflow = false;

    Coordinate_t neighbors[4] = {
        {lake_coord.x + 1, lake_coord.y},
        {lake_coord.x - 1, lake_coord.y},
        {lake_coord.x, lake_coord.y + 1},
        {lake_coord.x, lake_coord.y - 1}
    };

    for (const auto& neighbor_coord : neighbors) {
        if (neighbor_coord.x >= size.x || neighbor_coord.y >= size.y) {
            continue;
        }

        TileId_t neighbor_id = world.CoordinateToTileId(neighbor_coord);
        const Tile& neighbor = tiles[neighbor_id];

        // Only consider non-lake neighbors as overflow points
        if (!neighbor.GetIsLake()) {
            float neighbor_elevation = GetTileElevation(neighbor);
            if (neighbor_elevation < lowest_overflow_elevation) {
                lowest_overflow_elevation = neighbor_elevation;
                lowest_overflow = neighbor_coord;
                found_overflow = true;
            }
        }
    }

    out_overflow_coord = lowest_overflow;
    out_overflow_elevation = lowest_overflow_elevation;
    return found_overflow;
}

//! Process lake outflow when water level exceeds overflow elevation
void ProcessLakeOutflow(World& world) {
    auto& tiles = world.GetTiles();

    // Iterate through lakes and process outflow
    for (auto& lake_tile : tiles) {
        if (!lake_tile.GetIsLake()) {
            continue;
        }

        Coordinate_t lake_coord = world.TileIdToCoordinate(lake_tile.GetTileId());
        float lake_water_level = lake_tile.GetWaterLevel();

        // Find the overflow point
        Coordinate_t overflow_coord;
        float overflow_elevation;

        if (FindLakeOverflowPoint(world, lake_tile, lake_coord, overflow_coord, overflow_elevation)) {
            // If water level is above the overflow point, water spills out
            if (lake_water_level > overflow_elevation) {
                TileId_t overflow_id = world.CoordinateToTileId(overflow_coord);
                Tile& overflow_tile = tiles[overflow_id];

                // Calculate overflow amount (excess water above overflow point)
                float overflow_amount = lake_water_level - overflow_elevation;

                // Add outflow to the overflow tile
                float accumulated = overflow_tile.GetFlowAccumulation();
                overflow_tile.SetFlowAccumulation(accumulated + overflow_amount);

                // Set flow direction from lake to overflow point
                lake_tile.SetFlowDirection(overflow_coord);
            }
        }
    }
}

//! Identify and mark lakes at local minima
void IdentifyLakes(World& world) {
    auto& tiles = world.GetTiles();

    for (auto& tile : tiles) {
        if (tile.GetIsWater()) {
            continue;  // Skip ocean tiles
        }

        Coordinate_t coord = world.TileIdToCoordinate(tile.GetTileId());

        if (IsLocalMinimum(world, tile, coord) && tile.GetFlowAccumulation() > 1.0F) {
            tile.SetIsWater(true);
            tile.SetIsLake(true);
            tile.SetWaterLevel(tile.GetAbsoluteHeight());
        }
    }
}

//! Update lake water levels based on accumulated flow and process overflow
void UpdateLakeWaterLevels(World& world) {
    auto& tiles = world.GetTiles();

    // First pass: update water levels based on accumulated flow
    for (auto& lake_tile : tiles) {
        if (!lake_tile.GetIsLake()) {
            continue;
        }

        // Water level increases with accumulated flow
        // Use a simple scaling: additional water level = accumulated flow / 100
        float additional_water = lake_tile.GetFlowAccumulation() / 100.0F;
        float new_water_level = lake_tile.GetAbsoluteHeight() + additional_water;
        lake_tile.SetWaterLevel(new_water_level);
    }

    // Second pass: process outflow from lakes that have risen above their overflow point
    ProcessLakeOutflow(world);
}

//! Identify and mark rivers based on flow accumulation
void IdentifyRivers(std::vector<Tile>& tiles) {
    const float RIVER_THRESHOLD = 50.0F;

    for (auto& tile : tiles) {
        if (!tile.GetIsWater() && tile.GetFlowAccumulation() >= RIVER_THRESHOLD) {
            tile.SetIsRiver(true);
        }
    }
}

//! Update region water properties based on tiles
void UpdateRegionWaterProperties(World& world) {
    auto& regions = world.GetRegions();
    const auto& tiles = world.GetTiles();

    for (size_t region_idx = 0; region_idx < regions.size(); ++region_idx) {
        auto& region = regions[region_idx];
        bool has_water = false;
        bool has_river = false;

        for (const auto& tile : tiles) {
            if (tile.GetRegionId() == static_cast<RegionId_t>(region_idx)) {
                if (tile.GetIsWater()) {
                    has_water = true;
                }
                if (tile.GetIsRiver()) {
                    has_river = true;
                }
            }
        }

        region.SetIsOcean(has_water);
        region.SetHasRiver(has_river);
    }
}

void RunHydrologyPass(World& world, const WorldParams& params) {
    auto& tiles = world.GetTiles();

    // Step 1a: Determine ocean level
    float ocean_level = CalculateOceanLevel(tiles, params.GetPercentLand());

    // Step 1b: Mark ocean tiles
    MarkOceanTiles(tiles, ocean_level);

    // Step 1c & 1d: Initialize flow and trace water paths
    InitializeFlowAccumulation(tiles);
    std::vector<TileId_t> sorted_tile_ids = CreateSortedTilesByElevation(tiles);
    TraceWaterFlow(world, sorted_tile_ids);

    // Step 1e: Identify lakes
    IdentifyLakes(world);

    // Step 1e.5: Update lake water levels and process outflow
    UpdateLakeWaterLevels(world);

    // Step 1f: Identify rivers
    IdentifyRivers(tiles);

    // Step 1g: Update regions
    UpdateRegionWaterProperties(world);
}

}