#pragma once

#include "World.hpp"
#include <glm/vec2.hpp>
#include <string>

namespace World {

    // Use a layer based approach for world generation.
    class WorldGenerator {

        public:

            struct Params {

                //! ASCII string, whose hash is used as seed for random number generator.
                std::string m_seed;

                //! The dimmension used for the world.
                size_t m_dimmension;

                //! The number of continents in the world. Used in tectonic plate simulation
                size_t m_num_continents;

                //! Total percent of world that is land.
                float m_percent_land;

                //! The size of each region used for biome/feature assignment.
                size_t m_region_size;

            };


            WorldGenerator() = default;

            World Generate(WorldGenerator::Params& params);

        private:

    };
}