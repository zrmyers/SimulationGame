#pragma once

#include "MenuManager.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "ui/Button.hpp"
#include "ui/CheckBox.hpp"
#include "ui/Style.hpp"
#include "ui/Switch.hpp"
#include "ui/HorizontalLayout.hpp"
#include <memory>
#include <unordered_map>

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

            void SelectButton(SubmenuID submenuId);

            // Setup for submenus
            void BuildGameplaySettingsSubmenu(UI::HorizontalLayout& selectorPanel, UI::Switch& switcher);
            void BuildControlsSettingsSubmenu(UI::HorizontalLayout& selectorPanel, UI::Switch& switcher);
            void BuildGraphicsSettingsSubmenu(UI::HorizontalLayout& selectorPanel, UI::Switch& switcher);
            void BuildAudioSettingsSubmenu(UI::HorizontalLayout& selectorPanel, UI::Switch& switcher);
            void BuildAccessibilitySettingsSubmenu(UI::HorizontalLayout& selectorPanel, UI::Switch& switcher);

            // option that can be toggled
            void AddToggleOption(UI::Element& parent, const std::string& textLabel, bool initialState, UI::CheckBoxStateCallback_t callback);
            void AddDropdownMenu(UI::Element& parent, const std::string& textLabel, std::vector<std::string> choices, uint32_t selected);

        private:
            struct Submenu {
                UI::Button* p_button;
                size_t switch_index;
            };

            Core::Engine* m_p_engine;
            MenuManager* m_p_manager;
            std::shared_ptr<UI::Style> m_p_style;
            ECS::Entity m_entity;

            UI::Switch* m_p_submenu_switch;
            UI::Button* m_p_current_button;

            std::unordered_map<SubmenuID, Submenu> m_switch_index;

            // Graphics info
            std::vector<glm::ivec2> m_supported_resolutions;
    };
}