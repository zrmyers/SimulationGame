#include "Passes.hpp"

#include "math/PerlinNoise.hpp"
#include "world/Region.hpp"
#include "world/TectonicPlate.hpp"
#include "math/Voronoi.hpp"
#include "world/World.hpp"
#include "world/WorldParams.hpp"
#include <cstdint>
#include <glm/geometric.hpp>
#include <random>
#include <algorithm>

namespace World::Passes {

static void SmoothPass(std::vector<Region>& regions, int32_t iterations) {

    std::vector<float> newHeights;
    newHeights.reserve(regions.size());

    while (iterations > 0) {
        // Smooth the non-border regions a bit.
        for (Region& region : regions) {

            if (!region.GetIsBoundary()) {
                float count = 1.0F;
                float sum = region.GetAbsoluteHeight();
                for (RegionId_t neighborId : region.GetNeighbors())  {

                    Region& neighbor = regions.at(neighborId);
                    sum += neighbor.GetAbsoluteHeight();
                    count += 1.0F;
                }

                newHeights.push_back(sum / count);
            }
            else {
                newHeights.push_back(region.GetAbsoluteHeight());
            }
        }

        for (RegionId_t regionId = 0; regionId < regions.size(); regionId++) {
            regions.at(regionId).SetAbsoluteHeight(newHeights.at(regionId));
        }

        newHeights.clear();

        iterations--;
    }
}

void RunElevationPass(World& world, const WorldParams& params) {

    std::vector<TectonicPlate>& plates = world.GetPlates();
    std::vector<Region>& regions = world.GetRegions();
    std::vector<Tile>& tiles = world.GetTiles();
    Extent_t worldExtent = world.GetSize();

    Math::PerlinNoise perlin(params.GetSeed());

    // For each plate, assign average elevation based on whether the plate is continental or oceanic. In general,
    // oceanic plates should have lower elevation since they are denser, wheras continental should have higher elevation.
    // Use perlin noise to randomly assign the elevation based on band.
    for (TectonicPlate& plate : plates) {

        if (plate.GetIsContinental()) {

            // assign average continental height
            plate.SetAbsoluteHeight(4000.0F);
        }
        else {

            plate.SetAbsoluteHeight(2000.0F);
        }
    }

    //    b. For each region, use plate height, plate type, and boundary type to assign major geological features to region.
    //    c. Use geological feature and plate height to assign average height to region. The peak of the geological feature
    //       should follow the centroids of regions of same boundary on same plate.
    for (Region& region : regions) {

        TectonicPlate& plate = plates.at(region.GetPlateId());
        auto boundaryType = region.GetPlateBoundaryType();

        float baseHeight = plate.GetAbsoluteHeight();

        // sample the perlin noise
        glm::vec2 position = region.GetCentroid();
        Coordinate_t tileCoordinate = world.PositionToCoordinate(position);

        // calculate normalized position, givent tileCoordinate.
        glm::vec2 normalizedPos(
            static_cast<float>(tileCoordinate.x) / static_cast<float>(worldExtent.x),
            static_cast<float>(tileCoordinate.y) / static_cast<float>(worldExtent.y));

        float sample = perlin.Fbm(normalizedPos);

        switch (boundaryType.first) {

            case PlateBoundaryType::NONE:
                baseHeight += 1000.0F * sample - 500.0F; // +/- 500 m
                break;

            case PlateBoundaryType::TRANSFORM:
                baseHeight += 2000.0F * sample - 1000.0F; // +/- 1000 m
                break;

            case PlateBoundaryType::DIVERGENT:
                // Add trenches. minor lava flows also present
                baseHeight += -2000.0F * sample; // -0 to -2000 m
                break;

            case PlateBoundaryType::CONVERGENT:
                // Add mountains. if ocean is meeting continental, should add subduction on oceanic side.
                if (plate.GetIsContinental()) {
                    // create huge moutains
                    baseHeight += 2000.0F * (1.0F + sample); // 0 to 2000 m
                }
                else {
                    // create huge mountains if oceanic-oceanic, otherwise add trenches if oceanic-continental due to subduction.
                    TectonicPlate& otherPlate = plates.at(boundaryType.second);
                    if (otherPlate.GetIsContinental()) {

                        region.SetHasSubduction(true);
                        baseHeight -= 2000.0F * sample;
                    }
                    else {

                        baseHeight += 2000.0F * (1.0F + sample);
                    }

                }
                break;
        }

        region.SetAbsoluteHeight(baseHeight);
    }

    SmoothPass(regions, 3);

    //    d. Finally, use higher octave perlin noise to assign heights to each individual tile based on proximity to centroid
    //       or edge of region. Tiles closer to edge should blend with height of closest neighboring region.
    for (Tile& tile : tiles) {

        Region& region = regions.at(tile.GetRegionId());

        // normalize coordinate to world size
        Coordinate_t coordinate = world.TileIdToCoordinate(tile.GetTileId());
        glm::vec2 normalizedPos(
            static_cast<float>(coordinate.x) / static_cast<float>(worldExtent.x),
            static_cast<float>(coordinate.y) / static_cast<float>(worldExtent.y));


        tile.SetAbsoluteHeight(region.GetAbsoluteHeight() * perlin.Fbm(normalizedPos));
    }
}

} // namespace World::Passes
