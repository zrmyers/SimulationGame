#pragma once

#include "MenuManager.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "ui/Element.hpp"
#include "ui/Style.hpp"
#include <string>
#include <vector>
#include "world/WorldGenerator.hpp"

namespace Menu {

class CreateWorldMenu : public Menu::IMenu {

    public:
        CreateWorldMenu(Core::Engine& engine, MenuManager& manager, std::shared_ptr<UI::Style> p_style);

        void Activate() override;
        void Deactivate() override;

        void BuildCustomizationPanel(UI::Element& panelRoot);
        void BuildNavigationPanel(UI::Element& panelRoot);

    private:

        void GenerateWorld();

        Core::Engine* m_p_engine;
        MenuManager* m_p_manager;
        std::shared_ptr<UI::Style> m_p_style;

        ECS::Entity m_entity;

        World::WorldParams m_world_parameters;

        ECS::Entity m_sprite;

};

}
