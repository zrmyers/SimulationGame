#pragma once

#include "core/Engine.hpp"
#include "graphics/Mesh.hpp"
#include <cstddef>
#include <cstdint>
#include <glm/ext/vector_float4.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Character {

    // Character is composed of multiple parts.
    enum class PartId : uint8_t {
        HEAD = 0,
        UPPER_TORSO,
        HIP,
        RIGHT_ARM,
        LEFT_ARM,
        RIGHT_LEG,
        LEFT_LEG,
        RIGHT_FOOT,
        LEFT_FOOT,
        RIGHT_HAND,
        LEFT_HAND,
        EYES,
        EYELASHES,
        EYEBROW,
        HAIR,
        BEARD,

        // Add parts before
        PART_COUNT
    };

    static size_t PartIdToIndex(PartId part_id) {
        return static_cast<size_t>(part_id);
    }

    // Represents a part of the character
    struct BodyPart {
        PartId part_id; // Identifier for the part.
        std::shared_ptr<Graphics::Mesh> m_p_mesh; // graphical representation of part with mesh.
        int16_t m_max_health; // maximum possible health of the part.
        int16_t m_current_health; // current health of the part.
    };

    struct CharacterModel {
        std::unordered_map<PartId, std::shared_ptr<Graphics::Mesh>> m_healthy;
        std::unordered_map<PartId, std::shared_ptr<Graphics::Mesh>> m_destroyed_fresh;
        std::unordered_map<PartId, std::shared_ptr<Graphics::Mesh>> m_destroyed_scarred;
    };

    // This class is responsible for creating various instances of characters based on data from configuration file.
    class Creator {

        public:

            Creator(Core::Engine& engine, const std::string& filename);


            std::shared_ptr<CharacterModel> GetModel(const std::string& model_id);

        private:

            std::unordered_map<std::string, std::shared_ptr<CharacterModel>> m_models;

    };

    // This class represents a character in the game. Each character is assumed to be human, with the standard set of
    // body parts that a human is composed of. (consider moving definition to other class)
    //
    // each body part contributes to character health, and abilities.
    class Character {

        public:

            static std::unique_ptr<Character> CreateDefault(Core::Engine& engine);

            static constexpr size_t NUM_PARTS = static_cast<size_t>(PartId::PART_COUNT);

            Character() = default;

        private:

            // Customizable traits of character.
            glm::vec4 m_skin_color_primary {};
            glm::vec4 m_skin_color_secondary {};
            glm::vec4 m_hair_color {};
            glm::vec4 m_eye_color {};
            std::string m_hair_style; // if empty none
            std::string m_beard_style; // if empty none

            // set of parts that make up character.
            std::vector<BodyPart> m_parts;

            // reference to the character model
            std::shared_ptr<CharacterModel> m_model;
    };

}