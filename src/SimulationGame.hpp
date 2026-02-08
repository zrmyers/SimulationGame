#pragma once

#include "core/Engine.hpp"
#include "core/IGame.hpp"
#include "ecs/ECS.hpp"
#include "graphics/Font.hpp"
#include "menu/MenuManager.hpp"
#include "world/World.hpp"
#include <memory>

namespace World {
    class World;
}

class SimulationGame : public Core::IGame {

    public:

        explicit SimulationGame(Core::Engine& engine);

        void Update() override;

        void SetWorld(std::unique_ptr<World::World>&& p_world);
        World::World* GetWorld();

    private:

        void InitializeGUI();

        ECS::Entity m_camera_entity;

        Menu::MenuManager m_menu_manager;

        std::unique_ptr<World::World> m_p_world {nullptr};
};