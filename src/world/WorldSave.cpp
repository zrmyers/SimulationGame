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
#include <cstring>

namespace World {

static constexpr uint8_t WORLD_FILE_VERSION = 1;

// Binary serialization helpers
template<typename T>
static void WriteBinary(std::ofstream& stream, const T& value) {
    stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template<typename T>
static T ReadBinary(std::ifstream& stream) {
    T value;
    stream.read(reinterpret_cast<char*>(&value), sizeof(T));
    return value;
}

static void WriteString(std::ofstream& stream, const std::string& str) {
    uint32_t length = str.length();
    WriteBinary(stream, length);
    stream.write(str.c_str(), length);
}

static std::string ReadString(std::ifstream& stream) {
    uint32_t length = ReadBinary<uint32_t>(stream);
    std::string str(length, '\0');
    stream.read(&str[0], length);
    return str;
}

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

// Binary serialization functions
static void WriteParamsToBinary(std::ofstream& stream, const WorldParams& params) {
    WriteString(stream, params.GetName());
    WriteString(stream, params.GetSeedAscii());
    WriteBinary(stream, params.GetDimension());
    WriteBinary(stream, params.GetNumContinents());
    WriteBinary(stream, params.GetPercentLand());
    WriteBinary(stream, params.GetRegionSize());
}

static WorldParams ReadParamsFromBinary(std::ifstream& stream) {
    WorldParams params;
    params.SetName(ReadString(stream));
    params.SetSeedAscii(ReadString(stream));
    params.SetDimension(ReadBinary<size_t>(stream));
    params.SetNumContinents(ReadBinary<size_t>(stream));
    params.SetPercentLand(ReadBinary<float>(stream));
    params.SetRegionSize(ReadBinary<size_t>(stream));
    return params;
}

static void WritePlateToBinary(std::ofstream& stream, const TectonicPlate& plate) {
    WriteBinary(stream, plate.GetVelocity().x);
    WriteBinary(stream, plate.GetVelocity().y);
    WriteBinary(stream, plate.GetIsContinental());
    WriteBinary(stream, plate.GetAbsoluteHeight());
    WriteBinary(stream, plate.GetCentroid().x);
    WriteBinary(stream, plate.GetCentroid().y);

    const auto& boundaries = plate.GetBoundaries();
    WriteBinary(stream, static_cast<uint32_t>(boundaries.size()));
    for (const auto& boundary : boundaries) {
        WriteBinary(stream, boundary.first);
        WriteBinary(stream, static_cast<uint8_t>(boundary.second));
    }
}

static TectonicPlate ReadPlateFromBinary(std::ifstream& stream, World& world) {
    float velX = ReadBinary<float>(stream);
    float velY = ReadBinary<float>(stream);
    bool isContinental = ReadBinary<bool>(stream);
    float absoluteHeight = ReadBinary<float>(stream);
    float centroidX = ReadBinary<float>(stream);
    float centroidY = ReadBinary<float>(stream);

    TectonicPlate plate(world, glm::vec2(centroidX, centroidY));
    plate.SetVelocity(glm::vec2(velX, velY));
    plate.SetIsContinental(isContinental);
    plate.SetAbsoluteHeight(absoluteHeight);

    uint32_t boundaryCount = ReadBinary<uint32_t>(stream);
    for (uint32_t i = 0; i < boundaryCount; ++i) {
        int neighborPlateId = ReadBinary<int>(stream);
        uint8_t boundaryTypeByte = ReadBinary<uint8_t>(stream);
        plate.AddBoundary(neighborPlateId, static_cast<PlateBoundaryType>(boundaryTypeByte));
    }

    return plate;
}

static void WriteRegionToBinary(std::ofstream& stream, const Region& region) {
    WriteBinary(stream, region.GetPlateId());
    WriteBinary(stream, region.GetCentroid().x);
    WriteBinary(stream, region.GetCentroid().y);

    const auto& neighbors = region.GetNeighbors();
    WriteBinary(stream, static_cast<uint32_t>(neighbors.size()));
    for (RegionId_t neighbor : neighbors) {
        WriteBinary(stream, neighbor);
    }

    WriteBinary(stream, region.GetIsBoundary());
    WriteBinary(stream, region.GetHasSubduction());
    WriteBinary(stream, region.GetAbsoluteHeight());
    WriteBinary(stream, region.GetIsOcean());
    WriteBinary(stream, region.GetIsWater());
    WriteBinary(stream, region.GetIsLake());
    WriteBinary(stream, region.GetIsMountain());
    WriteBinary(stream, region.GetWaterLevel());
    WriteBinary(stream, region.GetFlowAccumulation());
    WriteBinary(stream, region.GetFlowDirection());
    WriteBinary(stream, region.GetHasRiver());
    WriteBinary(stream, region.GetTemperature());
    WriteBinary(stream, region.GetTemperatureVariance());
    WriteBinary(stream, region.GetMoisture());
    WriteString(stream, BiomeTypeToString(region.GetBiome()));
}

static Region ReadRegionFromBinary(std::ifstream& stream, World& world) {
    PlateId_t plateId = ReadBinary<PlateId_t>(stream);
    float centroidX = ReadBinary<float>(stream);
    float centroidY = ReadBinary<float>(stream);

    uint32_t neighborCount = ReadBinary<uint32_t>(stream);
    std::vector<RegionId_t> neighbors(neighborCount);
    for (uint32_t i = 0; i < neighborCount; ++i) {
        neighbors[i] = ReadBinary<RegionId_t>(stream);
    }

    Region region(world, glm::vec2(centroidX, centroidY), std::move(neighbors));
    region.SetPlateId(plateId);
    region.SetIsBoundary(ReadBinary<bool>(stream));
    region.SetHasSubduction(ReadBinary<bool>(stream));
    region.SetAbsoluteHeight(ReadBinary<float>(stream));
    region.SetIsOcean(ReadBinary<bool>(stream));
    region.SetIsWater(ReadBinary<bool>(stream));
    region.SetIsLake(ReadBinary<bool>(stream));
    region.SetIsMountain(ReadBinary<bool>(stream));
    region.SetWaterLevel(ReadBinary<float>(stream));
    region.SetFlowAccumulation(ReadBinary<float>(stream));
    region.SetFlowDirection(ReadBinary<RegionId_t>(stream));
    region.SetHasRiver(ReadBinary<bool>(stream));
    region.SetTemperature(ReadBinary<float>(stream));
    region.SetTemperatureVariance(ReadBinary<float>(stream));
    region.SetMoisture(ReadBinary<float>(stream));
    region.SetBiome(StringToBiomeType(ReadString(stream)));
    return region;
}

static void WriteTileToBinary(std::ofstream& stream, const Tile& tile) {
    WriteBinary(stream, tile.GetRegionId());
    WriteBinary(stream, tile.GetIsEdgeTile());
    WriteBinary(stream, tile.GetAbsoluteHeight());
    WriteBinary(stream, tile.GetIsWater());
    WriteBinary(stream, tile.GetIsRiver());
    WriteBinary(stream, tile.GetIsLake());
    WriteBinary(stream, tile.GetWaterLevel());
}

static void ReadTileFromBinary(std::ifstream& stream, World& world, TileId_t tileId) {
    Tile& tile = world.GetTile(tileId);
    tile.SetRegionId(ReadBinary<RegionId_t>(stream));
    tile.SetIsEdgeTile(ReadBinary<bool>(stream));
    tile.SetAbsoluteHeight(ReadBinary<float>(stream));
    tile.SetIsWater(ReadBinary<bool>(stream));
    tile.SetIsRiver(ReadBinary<bool>(stream));
    tile.SetIsLake(ReadBinary<bool>(stream));
    tile.SetWaterLevel(ReadBinary<float>(stream));
}

void SaveWorldToFile(const World& world) {

    const std::string worldName = world.GetParameters().GetName();
    std::string worldDir = CreateWorldDirectory(worldName);

    std::ofstream filestream(worldDir + "/world.bin", std::ios::binary);

    // Write magic number and version for validation
    filestream.write("WSAV", 4);
    uint8_t version = WORLD_FILE_VERSION;
    WriteBinary(filestream, version);

    // save parameters
    WriteParamsToBinary(filestream, world.GetParameters());

    // save plates
    uint32_t plateCount = world.GetPlates().size();
    WriteBinary(filestream, plateCount);
    for (const TectonicPlate& plate : world.GetPlates()) {
        WritePlateToBinary(filestream, plate);
    }

    // save regions
    uint32_t regionCount = world.GetRegions().size();
    WriteBinary(filestream, regionCount);
    for (const Region& region : world.GetRegions()) {
        WriteRegionToBinary(filestream, region);
    }

    // save tiles
    uint32_t tileCount = world.GetTiles().size();
    WriteBinary(filestream, tileCount);
    for (const Tile& tile : world.GetTiles()) {
        WriteTileToBinary(filestream, tile);
    }
}

std::unique_ptr<World> LoadWorldFromFile(const std::string& world_name) {

    std::string worldDir = GetWorldsDirectory() + world_name;
    std::ifstream filestream(worldDir + "/world.bin", std::ios::binary);
    if (!filestream.is_open()) {
        throw std::runtime_error("Failed to open world file: " + worldDir + "/world.bin");
    }

    // Verify magic number and version
    char magic[4];
    filestream.read(magic, 4);
    if (std::strncmp(magic, "WSAV", 4) != 0) {
        throw std::runtime_error("Invalid world save file format");
    }
    uint8_t version = ReadBinary<uint8_t>(filestream);
    if (version != WORLD_FILE_VERSION) {
        throw std::runtime_error("Unsupported world save version");
    }

    // Load parameters
    std::unique_ptr<World> world = std::make_unique<World>(ReadParamsFromBinary(filestream));

    // Load plates
    uint32_t plateCount = ReadBinary<uint32_t>(filestream);
    std::vector<TectonicPlate> plates;
    plates.reserve(plateCount);
    for (uint32_t i = 0; i < plateCount; ++i) {
        plates.push_back(ReadPlateFromBinary(filestream, *world));
    }
    world->SetPlates(std::move(plates));

    // Load Regions
    uint32_t regionCount = ReadBinary<uint32_t>(filestream);
    std::vector<Region> regions;
    regions.reserve(regionCount);
    for (uint32_t i = 0; i < regionCount; ++i) {
        regions.push_back(ReadRegionFromBinary(filestream, *world));
    }
    world->SetRegions(std::move(regions), false);

    // Load Tiles
    uint32_t tileCount = ReadBinary<uint32_t>(filestream);
    for (uint32_t tileId = 0; tileId < tileCount; ++tileId) {
        ReadTileFromBinary(filestream, *world, tileId);
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