#pragma once

#include <random>
#include <string>
#include <unordered_map>

namespace Math {

class NameGenerator {

    public:

        //! @brief Constructor for the name generator.
        NameGenerator(int32_t order, uint32_t seed);

        //! @brief Training data for the name generator.
        void Train(const std::vector<std::string>& names);

        //! @brief Generate a random name.
        std::string Generate(size_t max_characters);

    private:

        //! Set of markov chains used for name generation.
        std::unordered_map<std::string, std::vector<char>> m_chains;

        //! The size of the key used for sliding window of characters used for predicting the next character.
        int32_t m_order;

        //! The random number generator used for selecting next character in name;
        std::mt19937 m_generator;

        //! The string used for for starting generation of a character sequence.
        std::string m_start;
};
}