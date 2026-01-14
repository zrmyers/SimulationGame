#pragma once

#include "math/Voronoi.hpp"
#include <glm/vec2.hpp>
#include <string>

namespace World {

    // Use a layer based approach for world generation.
    // 0. Generate Regions
    // 3. Assign Rivers
    // 4. Assign Temperature and Moisture
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


            WorldGenerator();

            /**
             * @brief Generate a Voronoi subdivision and adjacency graph of regions.
             * @param regionCount Number of seed regions to generate.
             * @param canvasSize The size (width,height) of the generation area in arbitrary units.
             * @param sampleResolution Resolution used for rasterized nearest-seed sampling (higher => more accurate boundaries).
             * @param rngSeed Optional RNG seed for deterministic output (0 = non-deterministic).
             * @return VoronoiGraph containing regions and adjacency information.
             */
            Math::VoronoiGraph GenerateVoronoiRegions(
                int regionCount,
                const glm::ivec2& canvasSize,
                int sampleResolution,
                uint32_t rngSeed = 0U);

        private:

    };
}