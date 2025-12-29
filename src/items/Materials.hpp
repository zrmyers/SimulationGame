#pragma once

#include <cstdint>
#include <glm/vec4.hpp>
#include <string>

namespace Items {

    //! Index for material type.
    using MaterialIndex_t = uint16_t;

    //! Materials from which items can be created. Materials themselves can be harvested and stored in inventory as items.
    struct Material {

        //! Index of material.
        MaterialIndex_t m_id;

        //! Name of material.
        std::string m_name;

        //! Description of material.
        std::string m_description;

        //! Color of material.
        glm::vec4 m_color;
    };


    struct MaterialData {
        uint32_t m_type;
        uint32_t m_pad0;
        uint32_t m_pad1;
        uint32_t m_pad3;
        glm::vec4 m_color;
    };

    // MaterialInstance
    struct MaterialInstance {
        MaterialIndex_t m_index;
        MaterialData m_data;
    };
}