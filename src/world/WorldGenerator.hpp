#pragma once

#include "World.hpp"
#include "WorldParams.hpp"
#include <glm/vec2.hpp>

namespace World {


    // Use a layer based approach for world generation.
    class WorldGenerator {

        public:

            static World Generate(const WorldParams& params);

        private:

            // 1. Generate Tectonic Plates
            //    a. Generate a number of plates that is based off of number of continents and percent land parameter
            //    b. Randomly assign plate type, based off of number of contintents parameter.
            //    c. Assign 2D movement vectors to each plate.
            //    d. For each plate boundary, determine whether Convergent, Divergent, or Transform based on plate movement
            //    e. Generate gameplay regions which further subdivide plates.
            static void TectonicsPass(World& world, const WorldParams& params);
    };
}