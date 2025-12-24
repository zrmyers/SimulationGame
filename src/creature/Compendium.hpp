#pragma once

#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Texture2D.hpp"
#include "sdl/SDL.hpp"
#include <array>
#include <cstdint>
#include <fastgltf/types.hpp>
#include <glm/ext/vector_float4.hpp>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace Components {

    class CreatureInstance;
};

namespace Creature {

    //! Index for accessing material.
    using MaterialIndex_t = uint16_t;

    //! Index for accessing capability.
    using CapabilityIndex_t = uint16_t;

    //! Index for accessing a type of part.
    using PartTypeIndex_t = uint16_t;

    //! Index for a part option within a variant.
    using PartOptionIndex_t = uint16_t;

    //! Index for accessing species.
    using SpeciesIndex_t = uint16_t;

    //! Index for accessing variant.
    using VariantIndex_t = uint16_t;

    //! Maximum number of colors that can be input to a material.
    static constexpr uint16_t MAX_COLOR_INPUT_PER_MATERIAL = 3U;

    //! Creature capability. Describes the kind of actions a creature may perform.
    struct Capability {

        //! Identifier for the capability
        CapabilityIndex_t m_id;

        //! The name of the capability.
        std::string m_name;

        //! Description of the capability
        std::string m_description;
    };

    //! Shading Mode
    enum class ShaderType : uint8_t {
        SKIN = 0,
        EYES,
        HAIR
    };

    //! Input description
    struct MaterialInput {
        std::string m_name;
        std::string m_description;
    };

    //! Description of a material from which a creature is composed.
    struct Material {

        //! Index used for accessing material in species.
        MaterialIndex_t m_material_index;

        //! Type of shader to use for the material
        ShaderType m_shader_type;

        //! Name of the material
        std::string m_name;

        //! In-game description of material
        std::string m_description;

        //! Texture used for mixing provided input colors.
        std::unique_ptr<Graphics::Texture2D> m_p_color_map;

        //! Description of color slots for material.
        std::array<MaterialInput, Creature::MAX_COLOR_INPUT_PER_MATERIAL> m_colors;

        //! number of color slots used.
        uint16_t m_colors_count;

        //! Defaults
        std::array<glm::vec4, Creature::MAX_COLOR_INPUT_PER_MATERIAL> m_default_colors;
    };

    // MaterialInstance
    struct MaterialInstance {
        Creature::MaterialIndex_t m_index;
        std::array<glm::vec4, Creature::MAX_COLOR_INPUT_PER_MATERIAL> m_color_input;
    };

    //! Types of parts that make up a specific creature.
    struct PartType {

        //! ID of the part type
        PartTypeIndex_t m_id;

        //! The name of the part.
        std::string m_name;

        //! Description of the part.
        std::string m_description;

        //! Capabilities provided by the part.
        std::vector<const Capability*> m_p_provides;

        //! Material used by the part.
        Material* m_p_material;
    };

    //! Set of concrete parts from which only one may be chosen. I.e. used for selecting which model to use for a variant's
    //! hair.
    struct Part {

        //! The part type associated with the options.
        PartType* m_p_part_type;

        //! Options for meshes to use for displaying part.
        std::vector<std::unique_ptr<Graphics::Mesh>> m_p_options;

    };

    // Instances of parts used by creature.
    struct PartInstance {

        //! The type of part.
        Creature::PartTypeIndex_t m_part_type_index;

        //! Which variant of part is selected.
        Creature::PartOptionIndex_t m_part_option_index;

        //! Health of part.
        int16_t m_part_health;

        //! Whether the part is destroyed.
        bool m_is_destroyed;

        //! Entity used for rendering part
        ECS::Entity m_entity;
    };

    struct Skeleton {
        // Placeholder for skeleton used for animation.
        std::string m_name;
    };

    //! A variant of a species in the game.
    struct Variant {

        //! Index of variant within species.
        VariantIndex_t m_id;

        //! Name of the variant
        std::string m_name;

        //! Description of the variant.
        std::string m_description;

        //! The skeleton used for animation by the variant.
        Skeleton m_skeleton;

        //! Parts that make up the variant.
        std::vector<Part> m_parts;
    };

    struct Species {

        //! Index for accessing the record for the species
        SpeciesIndex_t m_id;

        //! Name of species
        std::string m_name;

        //! Description of the species.
        std::string m_description;

        //! set of materials used to define color for the species.
        std::vector<Material> m_materials;

        //! types of parts that make up creatures of the species.
        std::vector<PartType> m_part_types;

        //! Variants of a species. Each provides specific variants of parts defined by species.
        std::vector<Variant> m_variants;

        //! Get material by name
        Material& GetMaterialByName(const std::string& name);

        //! Get part type by name
        PartType& GetPartTypeByName(const std::string& name);

    };

    //! This class is responsible for providing a primary location for looking up information about various creatures in
    //! the game.
    class Compendium {

        public:

            //! Constructor for creature creator. Takes in a path to the creature compendium.
            Compendium() = default;

            void Load(Core::Engine& engine, const std::string& filename);

            const Species& GetSpeciesByIndex(SpeciesIndex_t index);
            const Species& GetSpeciesByName(const std::string& name);

            const Capability& GetCapabilityByName(const std::string& name) const;
            const Capability& GetCapabilityByIndex(CapabilityIndex_t index) const;

        private:

            //! Load materials for species.
            void LoadMaterials(Core::Engine& engine, Species& species, nlohmann::json& speciesData);

            //! Get the common texture sampler used for all creatures.
            std::shared_ptr<SDL::GpuSampler> GetTextureSampler(Core::Engine& engine);

            //! parse a color.
            static glm::vec4 ParseColor(nlohmann::json& colorData);

            //! Load creature part type information. This provides various metadata about creature, such as capabilities
            //!
            void LoadParts(Core::Engine& engine, Species& species, nlohmann::json& speciesData) const;

            //! Load species variant information.
            void LoadVariants(Core::Engine& engine, Species& species, nlohmann::json& speciesData) const;

            //! Load skeletal mesh
            static std::unique_ptr<Graphics::Mesh> LoadSkeletalMesh(Core::Engine& engine, fastgltf::Asset& asset, const std::string& nodeName);

            static fastgltf::Accessor& GetAccessor(fastgltf::Asset& asset, fastgltf::Primitive& primitive, const std::string& attributeName);

            //! Set of all capabilities for all creatures. These determine what kind of actions the creature may perform.
            std::vector<Capability> m_capabilities;

            //! Map of names to capabilities
            std::unordered_map<std::string, Capability*> m_capabilities_map;

            //! Set of all species encountered in game.
            std::vector<Species> m_species;

            //! Map of names to species
            std::unordered_map<std::string, Species*> m_species_map;

            //! Sampler Used for all creature textures
            std::shared_ptr<SDL::GpuSampler> m_p_sampler;
    };
}