#pragma once

#include "core/Engine.hpp"
#include "core/IGame.hpp"
#include "ecs/ECS.hpp"
#include "graphics/Font.hpp"
#include "menu/MenuManager.hpp"

class SimulationGame : public Core::IGame {

    public:

        explicit SimulationGame(Core::Engine& engine);

        void Update() override;

    private:

        void InitializeGUI();

        ECS::Entity m_camera_entity;

        Menu::MenuManager m_menu_manager;
};