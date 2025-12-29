#pragma once

#include "Materials.hpp"
#include "creature/Compendium.hpp"
#include "ecs/ECS.hpp"
#include "graphics/Mesh.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace Items {

    using ApparelIndex_t = uint16_t;
    using ApparelVariantIndex_t = uint16_t;

    enum ApparelLayers : uint8_t {
        SKIN = 0, // clothes
        MIDDLE, // armor
        OUTER // parkas, capes, jackets
    };

    // Mesh information for apparel variant.
    struct ApparelVariant {

        // Which variant of apparel.
        ApparelVariantIndex_t m_id;

        // Which species variant the apparel variant is compatible with.
        std::string m_compatible_species;

        //! Which variant of species the apparel can be worn by.
        std::string m_compatible_variant;

        // The mesh used for rendering the apparel.
        std::shared_ptr<Graphics::Mesh> m_p_mesh;
    };

    //! recipe for a type of apparel.
    struct Apparel {

        //! Index of apparel
        ApparelIndex_t m_id;

        //! Identifier for piece of equipment.
        std::string m_name;

        //! Description of the piece of equipment.
        std::string m_description;

        //! Layers associated with the equipment.
        std::vector<ApparelLayers> m_layers;

        //! Part coverage. Which parts are covered by the piece of equipment.
        std::vector<std::string> m_coverage;

        //! Variants of item, for compatibility with different creature meshes.
        std::vector<ApparelVariant> m_variants;
    };

    //! The instance of apparel
    struct ApparelInstance {

        //! What type of apparel.
        ApparelIndex_t m_index;

        //! The health of the pice of apparallel. When reaches 0, the item is destroyed.
        uint16_t m_durability;

        //! Which material is used for rendering.
        std::shared_ptr<MaterialInstance> m_p_material;

        //! ECS entity used for rendering apparel
        ECS::Entity m_entity;
    };
}