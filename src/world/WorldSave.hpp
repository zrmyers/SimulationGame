#pragma once

#include <string>
#include <memory>
#include <vector>

namespace World {

    class World;

    void SaveWorldToFile(const World& world);
    std::unique_ptr<World> LoadWorldFromFile(const std::string& world_name);

    std::vector<std::string> GetSavedWorlds();

    void DeleteWorld(const std::string& world_name);
}