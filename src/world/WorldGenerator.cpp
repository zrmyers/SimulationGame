/**
 * @file WorldGenerator.cpp
 * @brief WorldGenerator utilities (Voronoi region graph generation).
 */

#include "WorldGenerator.hpp"
#include "World.hpp"
#include "math/Voronoi.hpp"

namespace World {


World WorldGenerator::Generate(WorldGenerator::Params& params) {

    Extent_t worldExtent = {params.m_dimmension, params.m_dimmension};
    World world(worldExtent);

    // 1. Generate Tectonic Plates
    //    a. Generate a number of plates that is based off of number of continents and percent land parameter
    //    b. Randomly assign plate type, based off of number of contintents parameter.
    //    c. Assign 2D movement vectors to each plate.
    //    d. For each plate boundary, determine whether Convergent, Divergent, or Transform based on plate movement
    // 2. Generate Regions
    //    a. Generate a number of regions that subdivide the world.
    //    b. Assign each region to plate
    //    c. Assign terrain features to regions on plate boundaries.
    return world;
}

} // namespace World
