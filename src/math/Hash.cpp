#include "Hash.hpp"
#include <cstdint>

namespace Math {

    uint32_t HashFNV1A(const std::string &str) {

        static constexpr uint32_t FNV_OFFSET_BASIS = 0x811c9dc5;
        static constexpr uint32_t FNV_PRIME = 0x1000193;

        uint32_t hash = FNV_OFFSET_BASIS;

        for (char character : str) {
            hash = hash ^ static_cast<uint8_t>(character);
            hash *= FNV_PRIME;
        }

        return hash;
    }
}