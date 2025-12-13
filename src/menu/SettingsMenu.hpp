#pragma once

#include "MenuManager.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "ui/Button.hpp"
#include "ui/Style.hpp"
#include <memory>

namespace Menu {

    class SettingsMenu : public IMenu {

        public:

            enum class SubmenuID : uint8_t {
                GAMEPLAY = 0,
                CONTROLS,
                GRAPHICS,
                AUDIO,
                ACCESSIBILITY
            };

            SettingsMenu(Core::Engine& engine, MenuManager& manager, std::shared_ptr<UI::Style> p_style);

            void Activate() override;
            void Deactivate() override;

            void SelectButton(UI::Button& newButton);

        private:
            Core::Engine* m_p_engine;
            MenuManager* m_p_manager;
            std::shared_ptr<UI::Style> m_p_style;
            ECS::Entity m_entity;

            UI::Element* m_p_submenu_switch;
            UI::Button* m_p_current_button;
    };
}