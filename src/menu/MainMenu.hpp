#pragma once

#include "MenuManager.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "ui/Style.hpp"
#include <memory>

namespace Menu {

    class MainMenu : public IMenu {

        public:

            MainMenu(Core::Engine& engine, MenuManager& manager, std::shared_ptr<UI::Style> p_style);

            void Activate() override;
            void Deactivate() override;

        private:
            Core::Engine* m_p_engine;
            MenuManager* m_p_manager;
            std::shared_ptr<UI::Style> m_p_style;
            ECS::Entity m_entity;
    };
}