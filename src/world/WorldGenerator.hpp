#pragma once

#include "World.hpp"
#include "WorldParams.hpp"
#include <glm/vec2.hpp>
#include <memory>

namespace World {


    // Use a layer based approach for world generation.
    class WorldGenerator {

        public:

            static std::unique_ptr<World> Generate(const WorldParams& params);
    };
}