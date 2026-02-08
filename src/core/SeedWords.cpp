#include "SeedWords.hpp"
#include "Engine.hpp"
#include <fstream>
#include <string>
#include <vector>
#include "nlohmann/json.hpp"

namespace Core {

const std::vector<std::string>& SeedWords::GetSeedWords() {
    static std::vector<std::string> seedWords;

    if (seedWords.empty()) {

        std::ifstream filestream(Core::Engine::GetInstance().GetAssetLoader().GetDataDir() + "/names.json");
        nlohmann::json nameData = nlohmann::json::parse(filestream);
        for (const auto& name : nameData["SeedWords"]) {
            seedWords.push_back(name.get<std::string>());
        }

        if (seedWords.empty()) {
            seedWords.push_back("Coffee");
        }
    }

    return seedWords;
}

const std::string& SeedWords::ChooseRandomSeedWord() {
    const std::vector<std::string>& seedWords = GetSeedWords();

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<size_t> distribution(0, seedWords.size() - 1);
    size_t randomIndex = distribution(generator);
    return seedWords[randomIndex];
}

}