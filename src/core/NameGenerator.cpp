#include "NameGenerator.hpp"
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <nlohmann/json.hpp>
#include <random>

namespace Core {

NameGenerator NameGenerator::Load(const std::string& filepath, const std::string& name_type) {

    std::random_device rand;
    NameGenerator generator(2U, rand());

    // train the generator
    std::ifstream nameFile(filepath);
    nlohmann::json object = nlohmann::json::parse(nameFile);

    std::vector<std::string> trainingData = object[name_type];
    generator.Train(trainingData);
    return generator;
}

NameGenerator::NameGenerator(int32_t order, uint32_t seed)
    : m_order(order)
    , m_generator(seed)
    , m_start(m_order,'^') {
}

void NameGenerator::Train(const std::vector<std::string>& names) {
    for (const std::string& name : names) {

        std::string padded = m_start + name + '$';
        size_t numKeys = padded.length() - m_order; // number of keys generated from name

        for (size_t keyIndex = 0; keyIndex < numKeys; keyIndex++) {
            std::string key = padded.substr(keyIndex, m_order);
            m_chains[key].push_back(padded[keyIndex + m_order]);
        }
    }
}

std::string NameGenerator::Generate(size_t max_characters) {

    std::string current = m_start;
    std::string result;
    result.reserve(max_characters);

    while(result.length() < max_characters) {

        std::vector<char>& chooseFrom = m_chains[current];

        if (chooseFrom.empty()) {
            break;
        }

        std::uniform_int_distribution<uint32_t> choiceDistribution(0U, chooseFrom.size() - 1);
        char next = chooseFrom.at(choiceDistribution(m_generator));

        if (next == '$') {
            break; // reached end of word.
        }

        result += next;
        current = current.substr(1) + next;
    }

    return result;
}

}