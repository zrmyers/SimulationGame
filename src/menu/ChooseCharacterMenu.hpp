#pragma once

#include "MenuManager.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "ui/Style.hpp"
#include "ui/TextElement.hpp"
#include <cstddef>

namespace Menu {

class ChooseCharacterMenu : public Menu::IMenu {

    public:

        ChooseCharacterMenu(Core::Engine& engine, MenuManager& manager, std::shared_ptr<UI::Style> p_style);

        //! Select next/previous character.
        void SelectCharacter(bool next);

        //! Which character to select.
        void SelectCharacter(size_t index);

        //! Delete the currently selected character.
        void DeleteSelectedCharacter();

        void Activate() override;
        void Deactivate() override;

    private:

        Core::Engine* m_p_engine;
        MenuManager* m_p_manager;
        std::shared_ptr<UI::Style> m_p_style;

        ECS::Entity m_entity;

        size_t m_selected_character_index = 0U;
        std::vector<std::string> m_character_files;
        UI::TextElement* m_p_character_name_element {nullptr};

        ECS::Entity m_character_preview_entity;
};

}