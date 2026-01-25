#include "Passes.hpp"
#include "world/Region.hpp"
#include "world/World.hpp"
#include <vector>
#include <glm/geometric.hpp>
#include <algorithm>

namespace World::Passes {

static constexpr float TEMP_EQUATER = 40.0f;
static constexpr float TEMP_POLES = -30.0F;

static constexpr float MAX_VARIANCE = 20.0F;

//! Calculate temperature for a region based on latitude and elevation
std::pair<float, float> CalculateTemperature(Coordinate_t coordinate, Extent_t world_size, Region& region, float ocean_level) {
    // Base temperature: ~25°C at equator, decreases toward poles
    // Normalize latitude (y) to 0-1 range, where 0 is north pole and 1 is south pole
    float latitude = static_cast<float>(coordinate.y) / static_cast<float>(world_size.y);

    // Convert to -1 to 1 range, center at 0 (equator)
    float lat_offset = (latitude - 0.5F) * 2.0F;

    // Base temperature decreases with latitude (cosine wave: ~25°C at equator, ~-10°C at poles)
    float base_temp = ((TEMP_EQUATER - TEMP_POLES) * (1.0F - std::abs(lat_offset))) + TEMP_POLES;

    float variance = MAX_VARIANCE * std::abs(lat_offset);

    // Temperature decreases with elevation (~6.5°C per 1000m) above ocean level
    float elevation_temp_modifier = 0.0F;
    if (region.GetAbsoluteHeight() > ocean_level) {

        elevation_temp_modifier = -((region.GetAbsoluteHeight() - ocean_level)/ 1000.0F) * 6.5F;
    }

    return {base_temp + elevation_temp_modifier, variance};
}

//! Calculate moisture for a region based on proximity to water
float CalculateMoisture(const Region& region, const std::vector<Region>& all_regions) {

    // Moisture capacity is determined by temperature. We should calculate initial moisture capacity using the seeded
    // temperatures.

    // Once we know moisture capacity, we can give regions with higher temperature more moisture. Additionally, we'll have
    // to do a round of thermal dynamics to simulate effect of moisture on temperature.
    // Base moisture for water regions
    if (region.GetIsOcean() || region.GetIsLake()) {
        return 100.0F;
    }

    if (region.GetHasRiver()) {
        return 60.0F;
    }

    // For land regions, moisture decreases with distance from water
    // Check neighbors recursively up to distance 2
    float max_moisture = 0.0F;

    // Direct neighbors (distance 1)
    for (RegionId_t neighbor_id : region.GetNeighbors()) {
        if (neighbor_id == INVALID_REGION_ID) {
            continue;
        }

        const Region& neighbor = all_regions[neighbor_id];
        if (neighbor.GetIsLake()) {
            max_moisture = std::max(max_moisture, 80.0F);
        } else if (neighbor.GetHasRiver()) {
            max_moisture = std::max(max_moisture, 70.0F);
        }
    }

    // If no water neighbors, check secondary neighbors (distance 2)
    if (max_moisture == 0.0F) {
        for (RegionId_t neighbor_id : region.GetNeighbors()) {
            if (neighbor_id == INVALID_REGION_ID) {
                continue;
            }

            const Region& neighbor = all_regions[neighbor_id];
            for (RegionId_t neighbor2_id : neighbor.GetNeighbors()) {
                if (neighbor2_id == INVALID_REGION_ID) {
                    continue;
                }

                const Region& neighbor2 = all_regions[neighbor2_id];
                if (neighbor2.GetIsOcean() || neighbor2.GetIsLake()) {
                    max_moisture = std::max(max_moisture, 60.0F);
                } else if (neighbor2.GetHasRiver()) {
                    max_moisture = std::max(max_moisture, 50.0F);
                }
            }
        }
    }

    // Default moisture for inland regions
    if (max_moisture == 0.0F) {
        max_moisture = 30.0F;
    }

    return max_moisture;
}

void RunClimatePass(World& world, const WorldParams& params) {

    std::vector<Region>& regions = world.GetRegions();

    Extent_t worldSize = world.GetSize();

    // 1.a. Assign temperatures to regions based on proximity to poles and elevation.
    // 1.b. Assign moisture to regions based on proximity to water.
    for (auto& region : regions) {
        // Calculate temperature based on latitude and elevation
        std::pair<float, float> temperature = CalculateTemperature(
            world.PositionToCoordinate(region.GetCentroid()),
            worldSize,
            region,
            world.GetOceanLevel());
        region.SetTemperature(temperature.first);
        region.SetTemperatureVariance(temperature.second);

        // Calculate moisture based on proximity to water
        float moisture = CalculateMoisture(region, regions);
        region.SetMoisture(moisture);
    }

    // 1.c. Use Whitacker diagram to assign biomes to regions based on temperature and moisture.
    for (Region& region : regions) {

        float temperature = region.GetTemperature();
        float moisture = region.GetMoisture();

        // Use the biome dictionary to find the most specific type of biome that fits the region.
        if (region.GetIsOcean()) {

            // Assign one of the ocean biomes.
            if (temperature < -20.0F) {
                region.SetBiome(BiomeType::SEA_ICE);
            }
            else {
                region.SetBiome(BiomeType::OCEAN);
            }
        }
        else if (region.GetIsLake()) {

            if (temperature < -20.0F) {
                region.SetBiome(BiomeType::FROZEN_LAKE);
            }
            else {
                region.SetBiome(BiomeType::LAKE);
            }
        }
        else {

            // Cold Biomes
            if (temperature < -20.0F) {
                region.SetBiome(BiomeType::ICE_SHEET);
            }
            else if ((temperature < 0.0F) && ((moisture < 50.0F) || (region.GetIsMountain()))) {
                region.SetBiome(BiomeType::TUNDRA);
            }
            else if ((temperature < 10.0F) && ((moisture < 80.0F) || region.GetHasRiver())) {
                region.SetBiome(BiomeType::BOREAL_FOREST);
            }
            else if (temperature < 10.0F) {
                region.SetBiome(BiomeType::COLD_BOG);
            }
            else if (temperature > 27.5F) {

                if (moisture < 50.0F) {
                    region.SetBiome(BiomeType::EXTREME_DESERT);
                }
                if (moisture < 60.0F) {
                    region.SetBiome(BiomeType::DESERT);
                }
                else {
                    region.SetBiome(BiomeType::ARID_SHRUBLAND);
                }
            }
            else {

                bool is_swamp = ((moisture >= 80.0F) && !region.GetHasRiver());

                // Seperate out by variability in temperature
                if (region.GetTemperatureVariance() < 4.0F) {
                    if (is_swamp) {
                        region.SetBiome(BiomeType::TROPICAL_SWAMP);
                    }
                    else {
                        region.SetBiome(BiomeType::TROPICAL_RAINFOREST);
                    }
                }
                else if (is_swamp) {
                    region.SetBiome(BiomeType::TEMPERATE_SWAMP);
                } else {
                    region.SetBiome(BiomeType::TEMPERATE_FOREST);
                }
            }

        }
    }
}

}