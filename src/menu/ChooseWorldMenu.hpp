#pragma once

#include "MenuManager.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "ui/Style.hpp"
#include "ui/TextElement.hpp"
#include <vector>
#include <string>

namespace Menu {

class ChooseWorldMenu : public Menu::IMenu {

    public:

        ChooseWorldMenu(Core::Engine& engine, MenuManager& manager, std::shared_ptr<UI::Style> p_style);

        void Activate() override;
        void Deactivate() override;

        void SelectWorld(size_t worldIndex);
        void SelectWorld(bool next);

        void DeleteSelectedWorld();

    private:
        Core::Engine* m_p_engine;
        MenuManager* m_p_manager;
        std::shared_ptr<UI::Style> m_p_style;

        ECS::Entity m_entity;

        UI::TextElement* m_p_world_name_element {nullptr};

        size_t m_selected_world_index{0U};
        std::vector<std::string> m_worlds;
};

}
