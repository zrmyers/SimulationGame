#include "MapOverlay.hpp"
#include "World.hpp"
#include "Region.hpp"
#include "Tile.hpp"
#include "TectonicPlate.hpp"
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/vec4.hpp>
#include <glm/common.hpp>
#include <limits>

namespace World {

    std::vector<uint8_t> MapOverlay::GetOverlay(World& world, OverlayType overlayType) {
        switch (overlayType) {
            case OverlayType::PLATE_TECTONICS:
                return GetPlateTectonicsOverlay(world);
            case OverlayType::HEIGHT_MAP:
                return GetHeightMapOverlay(world);
            case OverlayType::WATER_MAP:
                return GetWaterMapOverlay(world);
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

            const Region& region = world.GetRegion(region_id);
            PlateId_t plate_id = region.GetPlateId();
            const TectonicPlate& plate = plates.at(plate_id);

            // Calculate pixel position
            Coordinate_t coord = world.TileIdToCoordinate(tile_id);
            size_t pixel_idx = (static_cast<size_t>(coord.y) * static_cast<size_t>(size.x) + static_cast<size_t>(coord.x)) * 4;

            // Determine if tile is in a plate boundary region
            bool is_in_plate_boundary = region.GetIsBoundary();
            bool is_region_boundary = tile.GetIsEdgeTile();
            bool is_continental = plate.GetIsContinental();
            auto boundaryType = region.GetPlateBoundaryType();

            // Set pixel color based on classification
            glm::u8vec4 color = is_continental?
                glm::u8vec4(0xc4U, 0xa4U, 0x84U,0xffU) : // NOLINT light brown
                glm::u8vec4(0xadU, 0xd8U, 0xe6U, 0xFF); // NOLINT aqua

            if (is_region_boundary && is_in_plate_boundary) {
                color = glm::u8vec4(0U);
            }
            else if (is_in_plate_boundary) {

                switch (boundaryType.first) {
                    case PlateBoundaryType::TRANSFORM:
                        color = glm::u8vec4(UINT8_MAX, 0U, 0U, UINT8_MAX);
                        break;

                    case PlateBoundaryType::DIVERGENT:
                        color = glm::u8vec4(0U, UINT8_MAX, 0U, UINT8_MAX);
                        break;

                    case PlateBoundaryType::CONVERGENT:
                        color = glm::u8vec4(0U, 0U, UINT8_MAX, UINT8_MAX);
                        break;

                    case PlateBoundaryType::NONE:
                        break;
                    }
            }
            // Write RGBA values
            buffer[pixel_idx + 0] = color.r;      // Red
            buffer[pixel_idx + 1] = color.g;      // Green
            buffer[pixel_idx + 2] = color.b;      // Blue
            buffer[pixel_idx + 3] = color.a;      // Alpha (opaque)
        }

        return buffer;
    }

    std::vector<uint8_t> MapOverlay::GetHeightMapOverlay(World& world) {
        Extent_t size = world.GetSize();
        size_t num_pixels = static_cast<size_t>(size.x) * static_cast<size_t>(size.y);

        // RGBA buffer - 4 bytes per pixel
        std::vector<uint8_t> buffer(num_pixels * 4);

        const std::vector<Tile>& tiles = world.GetTiles();

        // Find min and max heights for normalization
        float min_height = std::numeric_limits<float>::max();
        float max_height = std::numeric_limits<float>::lowest();

        for (const auto& tile : tiles) {
            float height = tile.GetAbsoluteHeight();
            min_height = glm::min(min_height, height);
            max_height = glm::max(max_height, height);
        }

        // Handle case where all heights are the same
        float height_range = max_height - min_height;
        if (height_range < 0.001F) {
            height_range = 1.0F;
        }

        // Process each tile
        for (const auto& tile : tiles) {
            TileId_t tile_id = tile.GetTileId();

            float height = tile.GetAbsoluteHeight();

            // Normalize height to [0.0, 1.0]
            float normalized_height = (height - min_height) / height_range;
            normalized_height = glm::clamp(normalized_height, 0.0F, 1.0F);

            // Convert to 0-255 range
            uint8_t pixel_value = static_cast<uint8_t>(normalized_height * 255.0F);

            // Calculate pixel position
            Coordinate_t coord = world.TileIdToCoordinate(tile_id);
            size_t pixel_idx = (static_cast<size_t>(coord.y) * static_cast<size_t>(size.x) + static_cast<size_t>(coord.x)) * 4;

            // Set grayscale RGBA values (all channels get the same value)
            buffer[pixel_idx + 0] = pixel_value;      // Red
            buffer[pixel_idx + 1] = pixel_value;      // Green
            buffer[pixel_idx + 2] = pixel_value;      // Blue
            buffer[pixel_idx + 3] = 255;              // Alpha (opaque)
        }

        return buffer;
    }

    std::vector<uint8_t> MapOverlay::GetWaterMapOverlay(World& world) {
        Extent_t size = world.GetSize();
        size_t num_pixels = static_cast<size_t>(size.x) * static_cast<size_t>(size.y);

        // RGBA buffer - 4 bytes per pixel
        std::vector<uint8_t> buffer(num_pixels * 4);

        const std::vector<Tile>& tiles = world.GetTiles();

        // Process each tile
        for (const auto& tile : tiles) {
            TileId_t tile_id = tile.GetTileId();
            Coordinate_t coord = world.TileIdToCoordinate(tile_id);
            size_t pixel_idx = (static_cast<size_t>(coord.y) * static_cast<size_t>(size.x) + static_cast<size_t>(coord.x)) * 4;

            glm::u8vec4 color;

            // Determine color based on water features
            if (tile.GetIsLake()) {
                // Cyan for lakes
                color = glm::u8vec4(0U, 255U, 255U, 255U);
            } else if (tile.GetIsRiver()) {
                // Light blue for rivers
                color = glm::u8vec4(100U, 149U, 237U, 255U);  // Cornflower blue
            } else if (tile.GetIsWater()) {
                // Dark blue for ocean
                color = glm::u8vec4(0U, 51U, 102U, 255U);  // Dark blue
            } else {
                // Green for land
                color = glm::u8vec4(34U, 139U, 34U, 255U);  // Forest green
            }

            // Write RGBA values
            buffer[pixel_idx + 0] = color.r;      // Red
            buffer[pixel_idx + 1] = color.g;      // Green
            buffer[pixel_idx + 2] = color.b;      // Blue
            buffer[pixel_idx + 3] = color.a;      // Alpha (opaque)
        }

        return buffer;
    }

}
