#include "MapOverlay.hpp"
#include "World.hpp"
#include "Region.hpp"
#include "Tile.hpp"
#include "TectonicPlate.hpp"
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/vec4.hpp>

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
            PlateBoundaryType boundaryType = region.GetPlateBoundaryType();

            // Set pixel color based on classification
            glm::u8vec4 color = is_continental?
                glm::u8vec4(0xc4U, 0xa4U, 0x84U,0xffU) : // NOLINT light brown
                glm::u8vec4(0xadU, 0xd8U, 0xe6U, 0xFF); // NOLINT aqua

            if (is_region_boundary && is_in_plate_boundary) {
                color = glm::u8vec4(0U);
            }
            else if (is_in_plate_boundary) {

                switch (boundaryType) {
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

}
