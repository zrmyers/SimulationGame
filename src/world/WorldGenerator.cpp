/**
 * @file WorldGenerator.cpp
 * @brief WorldGenerator utilities (Voronoi region graph generation).
 */

#include "WorldGenerator.hpp"
#include "math/Voronoi.hpp"

namespace World {

WorldGenerator::WorldGenerator() = default;

Math::VoronoiGraph WorldGenerator::GenerateVoronoiRegions(
    int regionCount,
    const glm::ivec2& canvasSize,
    int sampleResolution,
    uint32_t rngSeed) {

    return Math::VoronoiGenerator::Generate(regionCount, canvasSize, sampleResolution, rngSeed);
}

} // namespace World
