#pragma once

#include "MenuManager.hpp"
#include "ecs/ECS.hpp"
#include "ui/UI.hpp"
#include <memory>

namespace Menu {

    class MainMenu : public IMenu {

        public:

            MainMenu(MenuManager& manager, std::shared_ptr<UI::Style> p_style);

            void Activate() override;
            void Deactivate() override;

        private:

            MenuManager* m_p_manager;
            std::shared_ptr<UI::Style> m_p_style;
            ECS::Entity m_entity;
    };
}