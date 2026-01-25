#pragma once

#include <cstdint>
#include <glm/vec4.hpp>
#include <limits>
#include <queue>
#include <unordered_map>

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
}