#include "WorldSave.hpp"
#include "Region.hpp"
#include "Tile.hpp"
#include "WorldParams.hpp"
#include "core/Engine.hpp"
#include <fstream>
#include <unordered_map>
#include "nlohmann/json.hpp"
#include "World.hpp"
#include "Biome.hpp"
#include "core/Filesystem.hpp"
#include "world/TectonicPlate.hpp"

namespace World {

static std::string GetWorldsDirectory() {
    std::string worldsDir = Core::Engine::GetInstance().GetUserSaveDir() + "/worlds/";
    Core::Filesystem::CreateDirectory(worldsDir);
    return worldsDir;
}

static std::string CreateWorldDirectory(const std::string& worldName) {
    std::string worldDir = GetWorldsDirectory() + worldName;
    Core::Filesystem::CreateDirectory(worldDir);
    return worldDir;
}

static nlohmann::json ParamsToJson(const WorldParams& params) {

    nlohmann::json paramsData;

    paramsData["name"] = params.GetName();
    paramsData["seed_ascii"] = params.GetSeedAscii();
    paramsData["dimension"] = params.GetDimension();
    paramsData["num_continents"] = params.GetNumContinents();
    paramsData["percent_land"] = params.GetPercentLand();
    paramsData["region_size"] = params.GetRegionSize();

    return paramsData;
}

static WorldParams ParamsFromJson(const nlohmann::json& paramData) {

    WorldParams params;
    params.SetName(paramData["name"].get<std::string>());
    params.SetSeedAscii(paramData["seed_ascii"].get<std::string>());
    params.SetDimension(paramData["dimension"].get<int>());
    params.SetNumContinents(paramData["num_continents"].get<int>());
    params.SetPercentLand(paramData["percent_land"].get<float>());
    params.SetRegionSize(paramData["region_size"].get<int>());

    return params;
}

static nlohmann::json PlateToJson(const TectonicPlate& plate) {
    nlohmann::json plateData;

    plateData["velocity"] = {plate.GetVelocity().x, plate.GetVelocity().y};
    plateData["is_continental"] = plate.GetIsContinental();
    plateData["absolute_height"] = plate.GetAbsoluteHeight();
    plateData["centroid"] = {plate.GetCentroid().x, plate.GetCentroid().y};

    for (const auto& boundary : plate.GetBoundaries()) {
        std::string boundaryTypeStr;
        switch (boundary.second) {
            case PlateBoundaryType::NONE:
                boundaryTypeStr = "NONE";
                break;
            case PlateBoundaryType::TRANSFORM:
                boundaryTypeStr = "TRANSFORM";
                break;
            case PlateBoundaryType::DIVERGENT:
                boundaryTypeStr = "DIVERGENT";
                break;
            case PlateBoundaryType::CONVERGENT:
                boundaryTypeStr = "CONVERGENT";
                break;
        }

        plateData["boundaries"][std::to_string(boundary.first)] = boundaryTypeStr;
    }
    return plateData;
}

static TectonicPlate PlateFromJson(const nlohmann::json& plateData, World& world) {
    TectonicPlate plate(
        world,
        glm::vec2(plateData["centroid"][0].get<float>(), plateData["centroid"][1].get<float>()));

    plate.SetVelocity(glm::vec2(plateData["velocity"][0].get<float>(), plateData["velocity"][1].get<float>()));

    plate.SetIsContinental(plateData["is_continental"].get<bool>());
    plate.SetAbsoluteHeight(plateData["absolute_height"].get<float>());
    for (const auto& boundary : plateData["boundaries"].items()) {
        int neighborPlateId = std::stoi(boundary.key());
        std::string boundaryTypeStr = boundary.value().get<std::string>();
        PlateBoundaryType boundaryType = PlateBoundaryType::NONE;
        if (boundaryTypeStr == "NONE") {
            boundaryType = PlateBoundaryType::NONE;
        } else if (boundaryTypeStr == "TRANSFORM") {
            boundaryType = PlateBoundaryType::TRANSFORM;
        } else if (boundaryTypeStr == "DIVERGENT") {
            boundaryType = PlateBoundaryType::DIVERGENT;
        } else if (boundaryTypeStr == "CONVERGENT") {
            boundaryType = PlateBoundaryType::CONVERGENT;
        } else {
            throw std::runtime_error("Invalid plate boundary type: " + boundaryTypeStr);
        }
        plate.AddBoundary(neighborPlateId, boundaryType);
    }

    return plate;
}

static nlohmann::json RegionToJson(const Region& region) {
    nlohmann::json regionData;

    regionData["plate_id"] = region.GetPlateId();
    regionData["centroid"] = {region.GetCentroid().x, region.GetCentroid().y};
    regionData["neighbors"] = region.GetNeighbors();
    regionData["is_boundary"] = region.GetIsBoundary();
    regionData["has_subduction"] = region.GetHasSubduction();
    regionData["absolute_height"] = region.GetAbsoluteHeight();
    regionData["is_ocean"] = region.GetIsOcean();
    regionData["is_water"] = region.GetIsWater();
    regionData["is_lake"] = region.GetIsLake();
    regionData["is_mountain"] = region.GetIsMountain();
    regionData["water_level"] = region.GetWaterLevel();
    regionData["flow_accumulation"] = region.GetFlowAccumulation();
    regionData["flow_direction"] = region.GetFlowDirection();
    regionData["has_river"] = region.GetHasRiver();
    regionData["temperature"] = region.GetTemperature();
    regionData["temperature_variance"] = region.GetTemperatureVariance();
    regionData["moisture"] = region.GetMoisture();
    regionData["biome"] = BiomeTypeToString(region.GetBiome());

    return regionData;
}

static Region RegionFromJson(const nlohmann::json& regionData, World& world) {
    Region region(
        world,
        glm::vec2(regionData["centroid"][0].get<float>(), regionData["centroid"][1].get<float>()),
        regionData["neighbors"].get<std::vector<RegionId_t>>());
    region.SetPlateId(regionData["plate_id"].get<PlateId_t>());
    region.SetHasSubduction(regionData["has_subduction"].get<bool>());
    region.SetAbsoluteHeight(regionData["absolute_height"].get<float>());
    region.SetIsOcean(regionData["is_ocean"].get<bool>());
    region.SetIsWater(regionData["is_water"].get<bool>());
    region.SetIsLake(regionData["is_lake"].get<bool>());
    region.SetIsMountain(regionData["is_mountain"].get<bool>());
    region.SetWaterLevel(regionData["water_level"].get<float>());
    region.SetFlowAccumulation(regionData["flow_accumulation"].get<float>());
    region.SetFlowDirection(regionData["flow_direction"].get<RegionId_t>());
    region.SetHasRiver(regionData["has_river"].get<bool>());
    region.SetTemperature(regionData["temperature"].get<float>());
    region.SetTemperatureVariance(regionData["temperature_variance"].get<float>());
    region.SetMoisture(regionData["moisture"].get<float>());
    region.SetBiome(StringToBiomeType(regionData["biome"].get<std::string>()));
    return region;
}

static nlohmann::json TileToJson(const Tile& tile) {
    nlohmann::json tileData;

    tileData["region_id"] = tile.GetRegionId();
    tileData["is_edge"] = tile.GetIsEdgeTile();
    tileData["absolute_height"] = tile.GetAbsoluteHeight();
    tileData["is_water"] = tile.GetIsWater();
    tileData["is_river"] = tile.GetIsRiver();
    tileData["is_lake"] = tile.GetIsLake();
    tileData["water_level"] = tile.GetWaterLevel();

    return tileData;
}

static void TileFromJson(const nlohmann::json& tileData, World& world, TileId_t tileId) {
    Tile& tile = world.GetTile(tileId);
    tile.SetRegionId(tileData["region_id"].get<RegionId_t>());
    tile.SetIsEdgeTile(tileData["is_edge"].get<bool>());
    tile.SetAbsoluteHeight(tileData["absolute_height"].get<float>());
    tile.SetIsWater(tileData["is_water"].get<bool>());
    tile.SetIsRiver(tileData["is_river"].get<bool>());
    tile.SetIsLake(tileData["is_lake"].get<bool>());
    tile.SetWaterLevel(tileData["water_level"].get<float>());
}

void SaveWorldToFile(const World& world) {

    const std::string worldName = world.GetParameters().GetName();
    std::string worldDir = CreateWorldDirectory(worldName);

    Core::Engine& engine = Core::Engine::GetInstance();
    std::ofstream filestream(worldDir + "/world.json");
    nlohmann::json worldData;

    // save parameters
    worldData["parameters"] = ParamsToJson(world.GetParameters());

    // save plates
    worldData["plates"] = nlohmann::json::array();
    for (const TectonicPlate& plate : world.GetPlates()) {
        worldData["plates"].push_back(PlateToJson(plate));
    }

    // save regions
    worldData["regions"] = nlohmann::json::array();
    for (const Region& region : world.GetRegions()) {
        worldData["regions"].push_back(RegionToJson(region));
    }

    // save tiles
    worldData["tiles"] = nlohmann::json::array();
    for (const Tile& tile : world.GetTiles()) {
        worldData["tiles"].push_back(TileToJson(tile));
    }

    filestream << worldData.dump(4);
}

std::unique_ptr<World> LoadWorldFromFile(const std::string& world_name) {

    std::string worldDir = GetWorldsDirectory() + world_name;
    std::ifstream filestream(worldDir + "/world.json");
    if (!filestream.is_open()) {
        throw std::runtime_error("Failed to open world file: " + worldDir + "/world.json");
    }

    nlohmann::json worldData;
    filestream >> worldData;

    std::unique_ptr<World> world = std::make_unique<World>(ParamsFromJson(worldData["parameters"]));

    // Load plates
    std::vector<TectonicPlate> plates;
    plates.reserve(worldData["plates"].size());
    for (const auto& plateData : worldData["plates"]) {
        plates.push_back(PlateFromJson(plateData, *world));
    }
    world->SetPlates(std::move(plates));

    // Load Regions
    std::vector<Region> regions;
    regions.reserve(worldData["regions"].size());
    for (const auto& regionData : worldData["regions"]) {
        regions.push_back(RegionFromJson(regionData, *world));
    }

    // Load Tiles
    TileId_t tileId = 0;
    for (const auto& tileData : worldData["tiles"]) {
        TileFromJson(tileData, *world, tileId);
        tileId++;
    }

    return world;
}


std::vector<std::string> GetSavedWorlds() {
    std::vector<std::string> worlds;
    std::string worldsDir = GetWorldsDirectory();
    for (const auto& entry : std::filesystem::directory_iterator(worldsDir)) {
        if (entry.is_directory()) {
            worlds.push_back(entry.path().filename().string());
        }
    }
    return worlds;
}

void DeleteWorld(const std::string& world_name) {
    std::string worldDir = GetWorldsDirectory() + world_name;

    // Iterate through all files in world directory and delete them
    for (const auto& entry : std::filesystem::directory_iterator(worldDir)) {
        if (entry.is_regular_file()) {
            Core::Filesystem::DeletePath(entry.path().string());
        }
    }

    Core::Filesystem::DeletePath(worldDir);
}

}