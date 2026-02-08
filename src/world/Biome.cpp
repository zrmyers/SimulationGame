#pragma once

#include "Biome.hpp"
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace World {

    static constexpr const char* OCEAN_BIOME_NAME = "Ocean";
    static constexpr const char* LAKE_BIOME_NAME = "Lake";
    static constexpr const char* TEMPERATE_FOREST_BIOME_NAME = "Temperate Forest";
    static constexpr const char* TEMPERATE_SWAMP_BIOME_NAME = "Temperate Swamp";
    static constexpr const char* TROPICAL_RAINFOREST_BIOME_NAME = "Tropical Rainforest";
    static constexpr const char* TROPICAL_SWAMP_BIOME_NAME = "Tropical Swamp";
    static constexpr const char* ARID_SHRUBLAND_BIOME_NAME = "Arid Shrubland";
    static constexpr const char* DESERT_BIOME_NAME = "Desert";
    static constexpr const char* EXTREME_DESERT_BIOME_NAME = "Extreme Desert";
    static constexpr const char* BOREAL_FOREST_BIOME_NAME = "Boreal Forest";
    static constexpr const char* COLD_BOG_BIOME_NAME = "Cold Bog";
    static constexpr const char* TUNDRA_BIOME_NAME = "Tundra";
    static constexpr const char* ICE_SHEET_BIOME_NAME = "Ice Sheet";
    static constexpr const char* SEA_ICE_BIOME_NAME = "Sea Ice";
    static constexpr const char* FROZEN_LAKE_BIOME_NAME = "Frozen Lake";

    std::string BiomeTypeToString(BiomeType biome) {
        switch (biome) {
            case BiomeType::OCEAN:
                return OCEAN_BIOME_NAME;
            case BiomeType::LAKE:
                return LAKE_BIOME_NAME;
            case BiomeType::TEMPERATE_FOREST:
                return TEMPERATE_FOREST_BIOME_NAME;
            case BiomeType::TEMPERATE_SWAMP:
                return TEMPERATE_SWAMP_BIOME_NAME;
            case BiomeType::TROPICAL_RAINFOREST:
                return TROPICAL_RAINFOREST_BIOME_NAME;
            case BiomeType::TROPICAL_SWAMP:
                return TROPICAL_SWAMP_BIOME_NAME;
            case BiomeType::ARID_SHRUBLAND:
                return ARID_SHRUBLAND_BIOME_NAME;
            case BiomeType::DESERT:
                return DESERT_BIOME_NAME;
            case BiomeType::EXTREME_DESERT:
                return EXTREME_DESERT_BIOME_NAME;
            case BiomeType::BOREAL_FOREST:
                return BOREAL_FOREST_BIOME_NAME;
            case BiomeType::COLD_BOG:
                return COLD_BOG_BIOME_NAME;
            case BiomeType::TUNDRA:
                return TUNDRA_BIOME_NAME;
            case BiomeType::ICE_SHEET:
                return ICE_SHEET_BIOME_NAME;
            case BiomeType::SEA_ICE:
                return SEA_ICE_BIOME_NAME;
            case BiomeType::FROZEN_LAKE:
                return FROZEN_LAKE_BIOME_NAME;
            default:
                return {};
        }
    }

    BiomeType StringToBiomeType(const std::string& biomeStr) {

        static std::unordered_map<std::string, BiomeType> strToBiomeType = {
            {OCEAN_BIOME_NAME, BiomeType::OCEAN},
            {LAKE_BIOME_NAME, BiomeType::LAKE},
            {TEMPERATE_FOREST_BIOME_NAME, BiomeType::TEMPERATE_FOREST},
            {TEMPERATE_SWAMP_BIOME_NAME, BiomeType::TEMPERATE_SWAMP},
            {TROPICAL_RAINFOREST_BIOME_NAME, BiomeType::TROPICAL_RAINFOREST},
            {TROPICAL_SWAMP_BIOME_NAME, BiomeType::TROPICAL_SWAMP},
            {ARID_SHRUBLAND_BIOME_NAME, BiomeType::ARID_SHRUBLAND},
            {DESERT_BIOME_NAME, BiomeType::DESERT},
            {EXTREME_DESERT_BIOME_NAME, BiomeType::EXTREME_DESERT},
            {BOREAL_FOREST_BIOME_NAME, BiomeType::BOREAL_FOREST},
            {COLD_BOG_BIOME_NAME, BiomeType::COLD_BOG},
            {TUNDRA_BIOME_NAME, BiomeType::TUNDRA},
            {ICE_SHEET_BIOME_NAME, BiomeType::ICE_SHEET},
            {SEA_ICE_BIOME_NAME, BiomeType::SEA_ICE},
            {FROZEN_LAKE_BIOME_NAME, BiomeType::FROZEN_LAKE}
        };

        auto biomeIter = strToBiomeType.find(biomeStr);
        if (biomeIter != strToBiomeType.end()) {
            return biomeIter->second;
        }

        throw std::runtime_error("Invalid biome string: " + biomeStr);
    }
}