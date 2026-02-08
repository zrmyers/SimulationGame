#pragma once

#include <cstdint>
#include <glm/vec4.hpp>
#include <string>

namespace World {

    // Let's use Rimoworld's biome types to start with.
    enum class BiomeType : uint8_t {
        OCEAN = 0, // Default biome, moisture == 100
        LAKE,
        TEMPERATE_FOREST, // Warm biomes, seasons
        TEMPERATE_SWAMP, // wetter
        TROPICAL_RAINFOREST, // Warm biomes, no seasons
        TROPICAL_SWAMP, // wetter
        ARID_SHRUBLAND, // Hot Biomes
        DESERT,
        EXTREME_DESERT,
        BOREAL_FOREST, // Cold Biomes
        COLD_BOG,
        TUNDRA,
        ICE_SHEET,
        SEA_ICE,
        FROZEN_LAKE
    };

    static constexpr size_t BIOME_TYPE_COUNT = static_cast<size_t>(BiomeType::FROZEN_LAKE) + 1;

    std::string BiomeTypeToString(BiomeType biome);
    BiomeType StringToBiomeType(const std::string& biomeStr);
}