#pragma once

#include <string>
#include "glm/vec2.hpp"

namespace World {

    //! Each tile is 1024 meters across 1 KM. These means we can use 32 chunks of size 32x32 to render a single tile.
    static constexpr float TILE_SIZE_METERS_F32 = 1024.0F;
    static constexpr uint32_t TILE_SIZE_METERS_U32 = 1024U;
    static constexpr int32_t TILE_SIZE_METERS_S32 = 1024;

    static constexpr float TILE_PER_METER_F32 = 1.0F / TILE_SIZE_METERS_F32;

    //! Parameters used for world generation.
    class WorldParams {

        public:

            //! Set the ASCII seed string
            void SetSeedAscii(const std::string& seed);

            //! Get the ASCII seed string
            const std::string& GetSeedAscii() const;

            //! Get the seed value
            uint32_t GetSeed() const;

            //! Set the world dimension
            void SetDimension(size_t dimension);

            //! Get the world dimension
            size_t GetDimension() const;

            //! Get the world extent.
            glm::uvec2 GetWorldExtent() const;

            //! Set the number of continents
            void SetNumContinents(size_t numContinents);

            //! Get the number of continents
            size_t GetNumContinents() const;

            //! Set the percentage of land
            void SetPercentLand(float percentLand);

            //! Get the percentage of land
            float GetPercentLand() const;

            //! Set the region size
            void SetRegionSize(size_t regionSize);

            //! Get the region size
            size_t GetRegionSize() const;

            //! Calculate the number of tectonic plates
            int32_t CalculateNumPlates() const;

            //! Calculate the number of regions
            int32_t CalculateNumRegions() const;

        private:

            //! ASCII string, whose hash is used as seed for random number generator.
            std::string m_seed_ascii;

            //! Uint32 seed, calculated from string.
            uint32_t m_seed;

            //! The dimmension used for the world.
            size_t m_dimmension;

            //! The number of continents in the world. Used in tectonic plate simulation
            size_t m_num_continents;

            //! Total percent of world that is land.
            float m_percent_land;

            //! The size of each region used for biome/feature assignment.
            size_t m_region_size;

    };
}