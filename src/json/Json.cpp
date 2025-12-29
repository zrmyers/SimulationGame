#include "Json.hpp"
#include <glm/ext/vector_float4.hpp>
#include <nlohmann/json.hpp>

namespace JSON {

glm::vec4 ParseColor(nlohmann::json& colorData) {
    glm::vec4 color = {};
    color.r = colorData["r"];
    color.g = colorData["g"];
    color.b = colorData["b"];
    color.a = colorData["a"];
    return color;
}

}