#pragma once

#include "creature/Compendium.hpp"
#include <vector>

namespace Components {

    // This class represents a creature in the game. A creature is any kind of object in the game that has mechanical
    struct CreatureInstance {

        // Reference to species.
        Creature::Species* m_p_species{nullptr};

        // Material instances for creature. Determines how creature is colored during rendering.
        std::vector<std::shared_ptr<Creature::MaterialInstance>> m_instance;

        // Provides function to creature, and influences following:
        // - Capabilities/Skills
        // - Health
        // - Harvestable Resources
        // - Statistics
        std::vector<Creature::PartInstance> m_part_instances;

    };
}