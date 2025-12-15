#pragma once

#include <cstdlib>
#include <glm/vec2.hpp>

namespace Math {

    static int CalculateManhattenDistance(glm::ivec2 left, glm::ivec2 right) {

        return std::abs(left.x - right.x) + std::abs(left.y - right.y);
    }
}