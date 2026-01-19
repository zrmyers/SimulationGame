#pragma once

#include "world/World.hpp"
#include "world/WorldParams.hpp"

//! Provides single place to define entry points for each type of pass used during world generation.
namespace World::Passes {

    // 1. Generate Tectonic Plates
    //    a. Generate a number of plates that is based off of number of continents and percent land parameter
    //    b. Randomly assign plate type, based off of number of contintents parameter.
    //    c. Assign 2D movement vectors to each plate.
    //    d. For each plate boundary, determine whether Convergent, Divergent, or Transform based on plate movement
    //    e. Generate gameplay regions which further subdivide plates.
    void RunTectonicsPass(World& world, const WorldParams& params);

    // 2. Assign elevation values to map
    //    a. For each plate, assign average elevation based on whether the plate is continental or oceanic. In general,
    //       oceanic plates should have lower elevation since they are denser, wheras continental should have higher elevation.
    //       Use perlin noise to randomly assign the elevation based on band.
    //    b. For each region, use plate height, plate type, and boundary type to assign major geological features to region.
    //    c. Use geological feature and plate height to assign average height to region. The peak of the geological feature
    //       should follow the centroids of regions of same boundary on same plate.
    //    d. Finally, use higher octave perlin noise to assign heights to each individual tile based on proximity to centroid
    //       or edge of region. Tiles closer to edge should blend with height of closest neighboring region.
    void RunElevationPass(World& world, const WorldParams& params);

}