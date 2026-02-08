#pragma once

#include <vector>
#include <string>

namespace Core {

    class SeedWords {

        public:

            static const std::vector<std::string>& GetSeedWords();

            static const std::string& ChooseRandomSeedWord();
    };
}