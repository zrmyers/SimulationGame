#pragma once

#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include <string>
#include <unordered_map>

namespace Character {

    ECS::Entity LoadCharacterFromFile(const std::string& filename);
    void SaveCharacterToFile(const std::string& filename, const ECS::Entity& character);

    std::vector<std::string> GetSavedCharacterFiles();

    void DeleteCharacterFile(const std::string& filename);
}