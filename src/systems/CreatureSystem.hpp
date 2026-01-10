#pragma once

#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "creature/Compendium.hpp"
#include "graphics/pipelines/PipelineCache.hpp"

namespace Systems {

    //! The creature system is responsible for managing the appearance and animation of all creatures in the game.
    class CreatureSystem : public ECS::System {

        public:

            //! A constructor for the sprite system.
            CreatureSystem(Core::Engine& engine);

            //! The main update function.
            void Update() override;

            //! Make a createure of the given species and variant.
            Components::CreatureInstance MakeCreature(const std::string& species_name, Creature::VariantIndex_t variant_id);

            //! Make a creature of the species with the given sex.
            Components::CreatureInstance MakeCreature(const std::string& species_name, bool is_male = true);

            //! Get a species by name.
            const Creature::Species& GetSpeciesByName(const std::string& species_name) const;

        private:

            // Pipelines for rendering creatures
            Graphics::IPipeline* m_skeletal_mesh_pipeline;

            // Creature Compendium
            Creature::Compendium m_compendium;
    };
}