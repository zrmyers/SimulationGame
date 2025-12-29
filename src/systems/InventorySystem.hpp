#pragma once

#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "items/Apparel.hpp"
#include "items/ItemCatalog.hpp"
#include "graphics/pipelines/PipelineCache.hpp"

namespace Systems {

    //! The inventory system is responsible for managing inventories in the game.
    class InventorySystem : public ECS::System {

        public:

            //! A constructor for the inventory system
            InventorySystem(Core::Engine& engine);

            //! The main update function.
            void Update() override;

            Items::ItemCatalog& GetItemCatalog();

            //! Create an instance of apparel.
            Items::ApparelInstance MakeApparel(const std::string& apparel, const std::string& material);

        private:

            // Creature Compendium
            Items::ItemCatalog m_catalog;
    };
}