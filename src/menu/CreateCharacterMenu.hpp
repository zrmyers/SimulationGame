#pragma once

#include "MenuManager.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "ui/Element.hpp"
#include "ui/Style.hpp"
#include <memory>

namespace Menu {

class CreateCharacterMenu : public Menu::IMenu {

    public:

        CreateCharacterMenu(Core::Engine& engine, MenuManager& manager, std::shared_ptr<UI::Style> p_style);

        void Activate() override;
        void Deactivate() override;

    private:

        void BuildCustomizationPanel(UI::Element& panelRoot);
        void BuildFinalizationPanel(UI::Element& panelRoot);

        void SetCharacterSex(bool is_male);
        // Get the sex of the character. True if male, false if female.
        bool GetCharacterSex();

        Core::Engine* m_p_engine;
        MenuManager* m_p_manager;
        std::shared_ptr<UI::Style> m_p_style;

        ECS::Entity m_entity;
};

}