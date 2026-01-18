/**
 * @file WorldGenerator.cpp
 * @brief WorldGenerator utilities (Voronoi region graph generation).
 */

#include "WorldGenerator.hpp"
#include "Region.hpp"
#include "TectonicPlate.hpp"
#include "World.hpp"
#include "WorldParams.hpp"
#include "math/Voronoi.hpp"
#include <cstddef>
#include <cstdint>
#include <glm/ext/quaternion_geometric.hpp>
#include <random>

namespace World {


World WorldGenerator::Generate(const WorldParams& params) {

    Extent_t worldExtent = params.GetWorldExtent();
    World world(worldExtent);

    TectonicsPass(world, params);

    return world;
}


void WorldGenerator::TectonicsPass(World& world, const WorldParams& params) {

    std::vector<TectonicPlate> plates;
    std::vector<Region> regions;
    int32_t numPlates = params.CalculateNumPlates();
    int32_t numRegions = params.CalculateNumRegions();

    Extent_t canvasSize = world.GetSize() * TILE_SIZE_METERS_U32;

    plates.reserve(numPlates);
    regions.reserve(numRegions);

    Math::VoronoiGraph platesGraph = Math::VoronoiGenerator::Generate(
        numPlates,
        canvasSize,
        params.GetDimension(),
        params.GetSeed());
    Math::VoronoiGraph regionsGraph = Math::VoronoiGenerator::Generate(
        numRegions,
        canvasSize,
        params.GetDimension(),
        params.GetSeed());
    std::mt19937 rng;
    rng.seed(params.GetSeed());

    // plate movement distribution
    std::uniform_real_distribution<float> uniform_dist_x(0.0F, 1.0F);
    std::uniform_real_distribution<float> uniform_dist_y(0.0F, 1.0F);

    std::vector<PlateId_t> chooseFrom;
    chooseFrom.reserve(plates.size());

    // Create the plates, and initialize velocity
    for (int32_t plateId = 0; plateId < numPlates; plateId++) {

        TectonicPlate plate(world, platesGraph.m_centroids.at(plateId));

        glm::vec2 velocity = {uniform_dist_x(rng), uniform_dist_y(rng)};
        velocity = glm::normalize(velocity);
        plate.SetVelocity(velocity);

        chooseFrom.push_back(plateId);
        plates.push_back(plate);
    }

    // Assign continental plates. Randomly select plates until threshold met.
    size_t numContinents = 0U;
    while ((numContinents < params.GetNumContinents()) && !chooseFrom.empty()) {

        std::uniform_int_distribution<size_t> uniform_dist_choice(0U, chooseFrom.size() -1);

        int32_t chooseFromIndex = static_cast<int32_t>(uniform_dist_choice(rng));
        PlateId_t plateId = chooseFrom.at(chooseFromIndex);
        chooseFrom.erase(chooseFrom.begin() + chooseFromIndex);

        plates.at(plateId).SetIsContinental(true);
    }

    // Determine boundary types
    for (int32_t plateId =  0; plateId < numPlates; plateId++) {

        std::vector<PlateId_t> neighbors = platesGraph.m_adjacency.at(plateId);

        TectonicPlate& plateA = plates.at(plateId);
        for (PlateId_t neighborId : neighbors) {

            // Determine boundar type
            if (!plateA.HasBoundary(neighborId)) {

                TectonicPlate& plateB = plates.at(neighborId);
                PlateBoundaryType boundaryType = DeterminePlateBoundaryType(plateA, plateB);

                plateA.AddBoundary(neighborId, boundaryType);
                plateB.AddBoundary(plateId, boundaryType);
            }
        }
    }

    for (int32_t regionId = 0; regionId < numRegions; regionId++) {

        std::vector<RegionId_t> neighbors = regionsGraph.m_adjacency.at(regionId);
        Region region(world, regionsGraph.m_centroids.at(regionId),  std::move(neighbors));

        // determine plate membership of region
        int32_t plateId = static_cast<int32_t>(platesGraph.GetRegion(region.GetCentroid()));
        region.SetPlateId(plateId);
    }

    world.SetPlates(std::move(plates));
    world.SetRegions(std::move(regions));
}

} // namespace World
