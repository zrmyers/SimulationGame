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

        private:

            // Pipelines for rendering creatures
            Graphics::IPipeline* m_p_skin_pipeline;
            Graphics::IPipeline* m_p_hair_pipeline;
            Graphics::IPipeline* m_p_eyes_pipeline;

            // Creature Compendium
            Creature::Compendium m_compendium;
    };
}