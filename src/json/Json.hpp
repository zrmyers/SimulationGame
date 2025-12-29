#pragma once

#include <glm/ext/vector_float4.hpp>
#include <nlohmann/json_fwd.hpp>

namespace JSON {

    //! Helper to parse color from json object.
    glm::vec4 ParseColor(nlohmann::json& colorData);
}