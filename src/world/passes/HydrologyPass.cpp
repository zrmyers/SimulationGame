#include "Passes.hpp"

#include "world/Tile.hpp"
#include "world/Region.hpp"
#include "world/World.hpp"
#include <algorithm>
#include <glm/geometric.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <limits>
#include <queue>
#include <set>
#include <unordered_set>
#include <iostream>

namespace World::Passes {

//! Calculate ocean level by finding the height threshold that matches desired land percentage
float CalculateOceanLevel(const std::vector<Region>& regions, float percent_land) {
    if (regions.empty()) {
        return 0.0F;
    }

    // Collect all region elevations and sort them
    std::vector<float> elevations;
    elevations.reserve(regions.size());

    for (const auto& region : regions) {
        elevations.push_back(region.GetAbsoluteHeight());
    }

    std::sort(elevations.begin(), elevations.end());

    // Find the elevation threshold that gives us approximately the desired land distribution
    // We want to find the height where roughly (100 - percent_land)% of regions are below it (ocean)
    size_t target_ocean_index = static_cast<size_t>(static_cast<float>(regions.size()) * (1.0F - (percent_land * 0.01F)));
    target_ocean_index = glm::min(target_ocean_index, elevations.size() - 1);

    // Return the elevation at the target index
    return elevations[target_ocean_index];
}

//! Mark all regions below ocean level as water
void MarkOceanRegions(std::vector<Region>& regions, float ocean_level) {
    for (auto& region : regions) {
        if (region.GetAbsoluteHeight() < ocean_level) {
            region.SetIsOcean(true);
        }
    }
}

//! Initialize flow accumulation for all non-water regions
void InitializeRegionFlowAccumulation(std::vector<Region>& regions) {
    for (auto& region : regions) {
        if (!region.GetIsOcean()) {
            region.SetFlowAccumulation(1.0F);
        }
    }
}

//! Create a sorted list of region IDs ordered by elevation (highest to lowest)
std::vector<RegionId_t> CreateSortedRegionsByElevation(const std::vector<Region>& regions) {
    std::vector<RegionId_t> sorted_ids;
    sorted_ids.reserve(regions.size());

    for (size_t i = 0; i < regions.size(); ++i) {
        sorted_ids.push_back(static_cast<RegionId_t>(i));
    }

    std::sort(sorted_ids.begin(), sorted_ids.end(),
        [&regions](RegionId_t a, RegionId_t b) {
            return regions[a].GetAbsoluteHeight() > regions[b].GetAbsoluteHeight();
        });

    return sorted_ids;
}

//! Get the elevation of a region, accounting for water levels
float GetRegionElevation(const Region& region) {
    return region.GetIsOcean() ? region.GetWaterLevel() : region.GetAbsoluteHeight();
}

//! Find the lowest neighbor region and its elevation
//! Returns true if a lower neighbor was found
bool FindLowestNeighborRegion(const Region& current_region, const std::vector<Region>& all_regions,
                              RegionId_t& out_lowest_neighbor, float& out_lowest_elevation) {
    out_lowest_neighbor = INVALID_REGION_ID;
    out_lowest_elevation = GetRegionElevation(current_region);
    bool found_lower = false;

    for (RegionId_t neighbor_id : current_region.GetNeighbors()) {
        if (neighbor_id == INVALID_REGION_ID) {
            continue;
        }

        const Region& neighbor = all_regions[neighbor_id];
        float neighbor_elevation = GetRegionElevation(neighbor);

        if (neighbor_elevation < out_lowest_elevation) {
            out_lowest_elevation = neighbor_elevation;
            out_lowest_neighbor = neighbor_id;
            found_lower = true;
        }
    }

    return found_lower;
}

//! Trace water flow between regions and accumulate flow volumes
void TraceRegionWaterFlow(std::vector<Region>& regions, const std::vector<RegionId_t>& sorted_region_ids) {
    for (RegionId_t region_id : sorted_region_ids) {
        Region& region = regions[region_id];

        if (region.GetIsOcean()) {
            continue;  // Ocean regions don't flow further
        }

        RegionId_t lowest_neighbor;
        float lowest_elevation;

        if (FindLowestNeighborRegion(region, regions, lowest_neighbor, lowest_elevation)) {
            region.SetFlowDirection(lowest_neighbor);

            // Accumulate flow to the lowest neighbor
            Region& lowest_neighbor_region = regions[lowest_neighbor];
            float accumulated = lowest_neighbor_region.GetFlowAccumulation();
            lowest_neighbor_region.SetFlowAccumulation(accumulated + region.GetFlowAccumulation());
        }
    }
}

//! Check if a region is a local minimum (all neighbors are higher or equal elevation)
bool IsRegionLocalMinimum(const Region& region, const std::vector<Region>& all_regions) {
    float region_elevation = GetRegionElevation(region);

    for (RegionId_t neighbor_id : region.GetNeighbors()) {
        if (neighbor_id == INVALID_REGION_ID) {
            continue;
        }

        const Region& neighbor = all_regions[neighbor_id];
        float neighbor_elevation = GetRegionElevation(neighbor);

        if (neighbor_elevation <= region_elevation) {
            return false;
        }
    }

    return true;
}

//! Identify and mark lakes at local minima regions
void IdentifyLakeRegions(std::vector<Region>& regions) {
    for (auto& region : regions) {
        if (region.GetIsOcean()) {
            continue;  // Skip ocean regions
        }

        if (IsRegionLocalMinimum(region, regions) && region.GetFlowAccumulation() > 1.0F) {
            region.SetIsWater(true);
            region.SetIsLake(true);
            region.SetWaterLevel(region.GetAbsoluteHeight());
        }
    }
}

//! Find the lowest overflow point for a lake region
bool FindRegionLakeOverflowPoint(const Region& lake_region, const std::vector<Region>& all_regions,
                                 RegionId_t& out_overflow_region, float& out_overflow_elevation) {
    out_overflow_region = INVALID_REGION_ID;
    out_overflow_elevation = std::numeric_limits<float>::max();
    bool found_overflow = false;

    for (RegionId_t neighbor_id : lake_region.GetNeighbors()) {
        if (neighbor_id == INVALID_REGION_ID) {
            continue;
        }

        const Region& neighbor = all_regions[neighbor_id];

        // Only consider non-lake neighbors as overflow points
        if (!neighbor.GetIsLake()) {
            float neighbor_elevation = GetRegionElevation(neighbor);
            if (neighbor_elevation < out_overflow_elevation) {
                out_overflow_elevation = neighbor_elevation;
                out_overflow_region = neighbor_id;
                found_overflow = true;
            }
        }
    }

    return found_overflow;
}

//! Process lake outflow when water level exceeds overflow elevation
void ProcessRegionLakeOutflow(std::vector<Region>& regions) {
    for (auto& lake_region : regions) {
        if (!lake_region.GetIsLake()) {
            continue;
        }

        float lake_water_level = lake_region.GetWaterLevel();

        // Find the overflow point
        RegionId_t overflow_region_id;
        float overflow_elevation;

        if (FindRegionLakeOverflowPoint(lake_region, regions, overflow_region_id, overflow_elevation)) {
            // If water level is above the overflow point, water spills out
            if (lake_water_level > overflow_elevation) {
                Region& overflow_region = regions[overflow_region_id];

                // Calculate overflow amount (excess water above overflow point)
                float overflow_amount = lake_water_level - overflow_elevation;

                // Add outflow to the overflow region
                float accumulated = overflow_region.GetFlowAccumulation();
                overflow_region.SetFlowAccumulation(accumulated + overflow_amount);

                // Set flow direction from lake to overflow region
                lake_region.SetFlowDirection(overflow_region_id);
            }
        }
    }
}

//! Update lake water levels based on accumulated flow and process overflow
void UpdateLakeRegionWaterLevels(std::vector<Region>& regions) {
    // First pass: update water levels based on accumulated flow
    for (auto& lake_region : regions) {
        if (!lake_region.GetIsLake()) {
            continue;
        }

        // Water level increases with accumulated flow
        // Use a simple scaling: additional water level = accumulated flow / 100
        float additional_water = lake_region.GetFlowAccumulation() / 10.0F;
        float new_water_level = lake_region.GetAbsoluteHeight() + additional_water;
        lake_region.SetWaterLevel(new_water_level);
    }

    // Second pass: process outflow from lakes that have risen above their overflow point
    ProcessRegionLakeOutflow(regions);
}

//! Identify and mark rivers based on flow accumulation
void IdentifyRiverRegions(std::vector<Region>& regions) {
    const float RIVER_THRESHOLD = 5.0F;  // Threshold for river classification at region level

    for (auto& region : regions) {
        if (!region.GetIsOcean() && region.GetFlowAccumulation() >= RIVER_THRESHOLD) {
            region.SetHasRiver(true);
        }
    }
}

//! A* node for pathfinding
struct PathNode {
    Coordinate_t coord;
    float g_cost;  // Cost from start
    float h_cost;  // Heuristic cost to goal
    float f_cost;  // Total cost (g + h)
    Coordinate_t parent;
    bool operator>(const PathNode& other) const {
        return f_cost > other.f_cost;
    }
};

//! Trace river path using A* algorithm that prefers downhill paths
//! Returns set of tile IDs that form the path, or empty set if no path found
std::set<TileId_t> TraceRiverPathAStar(World& world, Coordinate_t start_coord, Coordinate_t end_coord) {
    int32_t iterations = 100U;
    std::set<TileId_t> result_path;
    auto& tiles = world.GetTiles();
    const Extent_t world_size = world.GetSize();

    // A* pathfinding
    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> frontier; // the set of nodes to explor next.
    std::unordered_set<Coordinate_t> visited; // set of nodes that have already been visited. These cannot be added back to frontier.
    std::unordered_map<Coordinate_t, Coordinate_t> came_from; // Used to trace the cheapest path from end point to start point.

    PathNode start_node{start_coord, 0.0F, 0.0F, 0.0F, start_coord};
    start_node.h_cost = glm::distance(glm::vec2(start_coord.x, start_coord.y), glm::vec2(end_coord.x, end_coord.y));
    start_node.f_cost = start_node.h_cost;
    frontier.push(start_node);

    const std::vector<glm::ivec2> neighbors_offsets = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},      // Cardinal directions
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}     // Diagonal directions
    };

    while (!frontier.empty() && (iterations >= 0)) {
        PathNode current = frontier.top();
        frontier.pop();

        if (current.coord.x == end_coord.x && current.coord.y == end_coord.y) {
            // Found the goal, reconstruct path
            Coordinate_t path_coord = end_coord;
            while (path_coord.x != start_coord.x || path_coord.y != start_coord.y) {
                TileId_t tile_id = world.CoordinateToTileId(path_coord);
                result_path.insert(tile_id);
                auto it = came_from.find({path_coord.x, path_coord.y});
                if (it != came_from.end()) {
                    path_coord = it->second;
                } else {
                    result_path.clear();
                    break;
                }
            }

            // Done, so let's return.
            break;
        }

        visited.insert({current.coord.x, current.coord.y});

        // Check all neighbors
        for (const auto& offset : neighbors_offsets) {
            Coordinate_t neighbor_coord{
                static_cast<int>(current.coord.x) + static_cast<int>(offset.x),
                static_cast<int>(current.coord.y) + static_cast<int>(offset.y)
            };

            // Check bounds
            if (neighbor_coord.x >= world_size.x || neighbor_coord.y >= world_size.y) {
                continue;
            }

            // Check if in closed set
            if (visited.count({neighbor_coord.x, neighbor_coord.y}) > 0U) {
                continue;
            }

            TileId_t current_tile_id = world.CoordinateToTileId(current.coord);
            TileId_t neighbor_tile_id = world.CoordinateToTileId(neighbor_coord);

            const Tile& current_tile = tiles[current_tile_id];
            const Tile& neighbor_tile = tiles[neighbor_tile_id];

            // Movement cost: diagonal moves cost more, and going downhill costs less
            float move_cost = (offset.x != 0 && offset.y != 0) ? 1.414F : 1.0F;  // Diagonal = sqrt(2)

            float height_diff = current_tile.GetAbsoluteHeight() - neighbor_tile.GetAbsoluteHeight();
            // Reward going downhill (reduce cost), penalize going uphill (increase cost)
            move_cost += height_diff * -0.1F;  // Negative because we want to reward downhill

            float g_cost = current.g_cost + move_cost;
            float h_cost = glm::distance(glm::vec2(neighbor_coord.x, neighbor_coord.y), glm::vec2(end_coord.x, end_coord.y));
            float f_cost = g_cost + h_cost;

            PathNode neighbor_node{neighbor_coord, g_cost, h_cost, f_cost, current.coord};
            frontier.push(neighbor_node);
            came_from[{neighbor_coord.x, neighbor_coord.y}] = current.coord;
        }

        iterations--;
    }

    return result_path;  // Empty set if no path found
}

//! Trace river path using Bresenham's line algorithm as fallback
//! Returns set of tile IDs that form the path
std::set<TileId_t> TraceRiverPathBresenham(World& world, Coordinate_t start_coord, Coordinate_t end_coord) {
    std::set<TileId_t> result_path;
    const Extent_t world_size = world.GetSize();

    int x0 = start_coord.x;
    int y0 = start_coord.y;
    int x1 = end_coord.x;
    int y1 = end_coord.y;

    int dx = std::abs(static_cast<int>(x1) - static_cast<int>(x0));
    int dy = std::abs(static_cast<int>(y1) - static_cast<int>(y0));
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (x0 >= 0 && x0 < static_cast<int>(world_size.x) && y0 >= 0 && y0 < static_cast<int>(world_size.y)) {
            TileId_t tile_id = world.CoordinateToTileId({x0, y0});
            result_path.insert(tile_id);
        }

        if (x0 == static_cast<int>(x1) && y0 == static_cast<int>(y1)) {
            break;
        }

        int e2 = 2 * err;
        if (e2 > -static_cast<int>(dy)) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < static_cast<int>(dx)) {
            err += dx;
            y0 += sy;
        }
    }

    return result_path;
}

//! Trace river path between two positions, preferring A* pathfinding with Bresenham fallback
void TraceTileLineRiver(World& world, glm::vec2 start_pos, glm::vec2 end_pos) {
    auto& tiles = world.GetTiles();
    const Extent_t world_size = world.GetSize();

    // Convert world positions to tile coordinates
    Coordinate_t start_coord(
        start_pos.x / TILE_SIZE_METERS_S32,
        start_pos.y / TILE_SIZE_METERS_S32
    );
    Coordinate_t end_coord(
        end_pos.x / TILE_SIZE_METERS_S32,
        end_pos.y / TILE_SIZE_METERS_S32
    );

    // Clamp to world bounds
    start_coord.x = glm::clamp(start_coord.x, 0U, world_size.x - 1U);
    start_coord.y = glm::clamp(start_coord.y, 0U, world_size.y - 1);
    end_coord.x = glm::clamp(end_coord.x, 0U, world_size.x - 1);
    end_coord.y = glm::clamp(end_coord.y, 0U, world_size.y - 1);

    // Try A* pathfinding first
    std::set<TileId_t> river_path = TraceRiverPathAStar(world, start_coord, end_coord);

    // Fallback to Bresenham if A* didn't find a path
    if (river_path.empty()) {
        river_path = TraceRiverPathBresenham(world, start_coord, end_coord);
    }

    // Mark all tiles in the path as rivers
    for (TileId_t tile_id : river_path) {
        Tile& tile = tiles[tile_id];
        if (!tile.GetIsWater()) {
            tile.SetIsRiver(true);
        }
    }
}

//! Map region-level water features to individual tiles
void MapRegionWaterFeaturesToTiles(World& world) {
    auto& tiles = world.GetTiles();
    const auto& regions = world.GetRegions();

    // First pass: map oceans and lakes to all tiles in those regions
    for (auto& tile : tiles) {
        RegionId_t region_id = tile.GetRegionId();

        if (region_id == INVALID_REGION_ID) {
            continue;
        }

        const Region& region = regions[region_id];

        // Map region water properties to tile
        if (region.GetIsOcean()) {
            tile.SetIsWater(true);
            tile.SetWaterLevel(region.GetWaterLevel());
        }

        if (region.GetIsLake()) {
            tile.SetIsLake(true);
            tile.SetIsWater(true);
            tile.SetWaterLevel(region.GetWaterLevel());
        }
    }

    // Second pass: trace river lines along flow directions
    for (size_t region_idx = 0; region_idx < regions.size(); ++region_idx) {
        const Region& region = regions[region_idx];

        if (!region.GetHasRiver()) {
            continue;
        }

        RegionId_t flow_target = region.GetFlowDirection();
        if (flow_target == INVALID_REGION_ID) {
            continue;
        }

        // Trace a line of river tiles from this region's centroid to the target region's centroid
        glm::vec2 start_pos = region.GetCentroid();
        glm::vec2 end_pos = regions[flow_target].GetCentroid();

        TraceTileLineRiver(world, start_pos, end_pos);
    }
}

void RunHydrologyPass(World& world, const WorldParams& params) {
    auto& regions = world.GetRegions();

    // Step 1a: Determine ocean level based on region elevations
    float ocean_level = CalculateOceanLevel(regions, params.GetPercentLand());

    world.SetOceanLevel(ocean_level);

    // Step 1b: Mark ocean regions
    MarkOceanRegions(regions, ocean_level);

    // Step 1c & 1d: Initialize flow and trace water paths between regions
    InitializeRegionFlowAccumulation(regions);
    std::vector<RegionId_t> sorted_region_ids = CreateSortedRegionsByElevation(regions);
    TraceRegionWaterFlow(regions, sorted_region_ids);

    // Step 1e: Identify lake regions
    IdentifyLakeRegions(regions);

    // Step 1e.5: Update lake water levels and process outflow
    UpdateLakeRegionWaterLevels(regions);

    // Step 1f: Identify river regions
    IdentifyRiverRegions(regions);

    // Step 1g: Map region-level features to tiles
    MapRegionWaterFeaturesToTiles(world);
}

}  // namespace World::Passes