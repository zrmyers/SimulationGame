#include "MapOverlay.hpp"
#include "World.hpp"
#include "Region.hpp"
#include "Tile.hpp"
#include "TectonicPlate.hpp"

namespace World {

    std::vector<uint8_t> MapOverlay::GetOverlay(World& world, OverlayType overlayType) {
        switch (overlayType) {
            case OverlayType::PLATE_TECTONICS:
                return GetPlateTectonicsOverlay(world);
            default:
                // Return black overlay for unknown types
                Extent_t size = world.GetSize();
                return std::vector<uint8_t>(size.x * size.y * 4, 0);
        }
    }

    std::vector<uint8_t> MapOverlay::GetPlateTectonicsOverlay(World& world) {
        Extent_t size = world.GetSize();
        size_t num_pixels = static_cast<size_t>(size.x) * static_cast<size_t>(size.y);

        // RGBA buffer - 4 bytes per pixel
        std::vector<uint8_t> buffer(num_pixels * 4, 255);  // Initialize to white with alpha = 255

        const std::vector<Tile>& tiles = world.GetTiles();
        const std::vector<Region>& regions = world.GetRegions();
        const std::vector<TectonicPlate>& plates = world.GetPlates();

        // Process each tile
        for (const auto& tile : tiles) {
            TileId_t tile_id = tile.GetTileId();
            RegionId_t region_id = tile.GetRegionId();

            // Skip invalid regions
            if (region_id == INVALID_REGION_ID) {
                continue;
            }

            const Region& region = world.GetRegion(region_id);
            PlateId_t plate_id = region.GetPlateId();

            // Skip invalid plates
            if (plate_id == INVALID_PLATE_ID) {
                continue;
            }

            // Calculate pixel position
            Coordinate_t coord = world.TileIdToCoordinate(tile_id);
            size_t pixel_idx = (static_cast<size_t>(coord.y) * static_cast<size_t>(size.x) + static_cast<size_t>(coord.x)) * 4;

            if (pixel_idx + 3 >= buffer.size()) {
                continue;
            }

            // Determine if this tile is on a boundary
            bool is_boundary_tile = false;
            PlateBoundaryType boundary_type = PlateBoundaryType::TRANSFORM;

            // Check if any neighbor is in a different region (indicates a region edge)
            bool is_region_edge = false;

            // For simplicity, we can check neighboring tiles
            // Check 4 neighbors (up, down, left, right)
            Coordinate_t neighbors[4] = {
                {coord.x, coord.y + 1},
                {coord.x, coord.y - 1},
                {coord.x + 1, coord.y},
                {coord.x - 1, coord.y}
            };

            for (const auto& neighbor_coord : neighbors) {
                // Check bounds
                if (neighbor_coord.x >= size.x || neighbor_coord.y >= size.y) {
                    continue;
                }

                TileId_t neighbor_tile_id = world.CoordinateToTileId(neighbor_coord);
                const Tile& neighbor_tile = world.GetTile(neighbor_tile_id);
                RegionId_t neighbor_region_id = neighbor_tile.GetRegionId();

                // If neighboring tile is in a different region, this is an edge
                if (neighbor_region_id != region_id && neighbor_region_id != INVALID_REGION_ID) {
                    is_region_edge = true;

                    // Get the neighbor region and its plate
                    const Region& neighbor_region = world.GetRegion(neighbor_region_id);
                    PlateId_t neighbor_plate_id = neighbor_region.GetPlateId();

                    // If on a different plate, determine boundary type
                    if (neighbor_plate_id != plate_id && neighbor_plate_id != INVALID_PLATE_ID) {
                        is_boundary_tile = true;
                        const TectonicPlate& current_plate = world.GetPlate(plate_id);
                        const TectonicPlate& neighbor_plate = world.GetPlate(neighbor_plate_id);
                        boundary_type = DeterminePlateBoundaryType(current_plate, neighbor_plate);
                        break;
                    }
                }
            }

            // Set pixel color based on classification
            uint8_t r = 255, g = 255, b = 255, a = 255;  // Default: white

            if (is_region_edge && !is_boundary_tile) {
                // Black for region edges that are not plate boundaries
                r = 0;
                g = 0;
                b = 0;
            } else if (is_boundary_tile) {
                // Color based on plate boundary type
                switch (boundary_type) {
                    case PlateBoundaryType::TRANSFORM:
                        // Red for transform boundaries
                        r = 255;
                        g = 0;
                        b = 0;
                        break;
                    case PlateBoundaryType::CONVERGENT:
                        // Green for convergent boundaries
                        r = 0;
                        g = 255;
                        b = 0;
                        break;
                    case PlateBoundaryType::DIVERGENT:
                        // Blue for divergent boundaries
                        r = 0;
                        g = 0;
                        b = 255;
                        break;
                }
            }

            // Write RGBA values
            buffer[pixel_idx + 0] = r;      // Red
            buffer[pixel_idx + 1] = g;      // Green
            buffer[pixel_idx + 2] = b;      // Blue
            buffer[pixel_idx + 3] = a;      // Alpha (opaque)
        }

        return buffer;
    }

}
