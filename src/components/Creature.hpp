#pragma once

#include "creature/Compendium.hpp"
#include "items/Apparel.hpp"
#include <vector>
#include "core/String.hpp"

namespace Components {

    // This class represents a creature in the game. A creature is any kind of object in the game that is able to perform
    // actions.
    struct CreatureInstance {

        // Reference to species.
        const Creature::Species* m_p_species{nullptr};

        // Which variant of species
        Creature::VariantIndex_t m_variant_id;

        // Material instances for creature. Determines how creature is colored during rendering.
        std::vector<Creature::MaterialInstance> m_material_instance;

        // Provides function to creature, and influences following:
        // - Capabilities/Skills
        // - Health
        // - Harvestable Resources
        // - Statistics
        std::vector<Creature::PartInstance> m_part_instances;

        // Equipment on character to render.
        std::vector<Items::ApparelInstance> m_equipment;

        // Various creature attachments. These are similar to parts, except have transformation to particular location on mesh,
        // as they do not share a comment origin with the creature.
        std::vector<Creature::AttachmentInstance> m_attachments;

        //! Returns true if male, otherwise false if female.
        bool GetSex() const {

            return Core::EndsWith(m_p_species->m_variants.at(m_variant_id).m_name, "-male");
        }

    };
}