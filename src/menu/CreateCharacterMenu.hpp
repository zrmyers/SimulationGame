#pragma once

#include "MenuManager.hpp"
#include "MenuUtilities.hpp"
#include "core/Engine.hpp"
#include "creature/Compendium.hpp"
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

        void AddMaterialCustomizer(UI::Element& root,const Creature::Material& material, const std::string& fieldName, SelectionChangeCallback_t callback);

        void AddPartCustomizer(UI::Element& root, const Creature::PartType& partType, const std::string& fieldName, SelectionChangeCallback_t callback);

        // Set the sex of the character.
        void SetCharacterSex(bool is_male);

        // Get the sex of the character. True if male, false if female.
        bool GetCharacterSex();

        // Set the skin tone of the character.
        void SetSkinColor(size_t selected_pallete);

        // Set the eye color of the character.
        void SetEyeColor(size_t selected_pallete);

        // Set the hair color of the character.
        void SetHairColor(size_t selected_pallete);

        // Set the hair style of the character.
        void SetHairStyle(size_t selected_style);

        // Set the name of the character.
        void SetName(const std::string& name);

        // Save character to file.
        void SaveCharacter();

        // Process mouse movement
        void ProcessMouseMovement();

        Core::Engine* m_p_engine;
        MenuManager* m_p_manager;
        std::shared_ptr<UI::Style> m_p_style;

        ECS::Entity m_entity;

        bool m_track_mouse;
        glm::vec2 m_prev_position;
        glm::mat4 m_original_transform;

        UI::Button* m_p_done_button;
};

}