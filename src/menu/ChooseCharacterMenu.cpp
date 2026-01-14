#include "ChooseCharacterMenu.hpp"
#include "characters/PlayerCharacter.hpp"
#include "MenuUtilities.hpp"
#include "components/Canvas.hpp"
#include "components/Creature.hpp"
#include "components/Transform.hpp"
#include "core/Logger.hpp"
#include "ecs/ECS.hpp"
#include "ui/Button.hpp"
#include "ui/ButtonStyle.hpp"
#include "ui/HorizontalLayout.hpp"
#include "ui/Spacer.hpp"
#include "ui/TextElement.hpp"
#include "ui/VerticalLayout.hpp"

Menu::ChooseCharacterMenu::ChooseCharacterMenu(Core::Engine& engine, MenuManager& manager, std::shared_ptr<UI::Style> p_style)
    : m_p_engine(&engine)
    , m_p_manager(&manager)
    , m_p_style(std::move(p_style)) {
}

void Menu::ChooseCharacterMenu::Activate() {

    Core::Engine* p_engine = m_p_engine;
    MenuManager* p_menuManager = m_p_manager;
    ECS::Registry& registry = m_p_engine->GetEcsRegistry();

    m_entity = ECS::Entity(registry);

    // Get list of characters
    m_character_files = Character::GetSavedCharacterFiles();
    m_selected_character_index = 0U;

    Components::Canvas& canvas = m_entity.EmplaceComponent<Components::Canvas>();
    canvas.SetRenderMode(Components::Canvas::RenderMode::SCREEN);
    // populate canvas with ui elements

    UI::VerticalLayout& vertical = canvas.EmplaceChild<UI::VerticalLayout>();

    vertical.EmplaceChild<UI::Spacer>();  // top of screen should be empty space

    // This is bar for choosing between various saved characters, if they exist
    UI::HorizontalLayout& characterRotary = vertical.EmplaceChild<UI::HorizontalLayout>();
    characterRotary.EmplaceChild<UI::Spacer>();
    AddButton(
        m_p_style,
        characterRotary,
        "<",
        m_character_files.empty()? UI::ButtonState::DISABLED : UI::ButtonState::ENABLED,
        [this](){ this->SelectCharacter(false); });
    UI::TextElement& characterName = AddTextElement(
        m_p_style,
        characterRotary,
        "None",
        glm::vec4(1.0F, 1.0F, 1.0F, 1.0F),
        20U);
    m_p_character_name_element = &characterName;

    AddButton(
        m_p_style,
        characterRotary,
        ">",
        m_character_files.empty()? UI::ButtonState::DISABLED : UI::ButtonState::ENABLED,
        [this](){ this->SelectCharacter(true); });

    characterRotary.EmplaceChild<UI::Spacer>();

    // menu navigation and character management bar
    UI::HorizontalLayout& characterManagement = vertical.EmplaceChild<UI::HorizontalLayout>();
    characterManagement.EmplaceChild<UI::Spacer>();
    AddButton(m_p_style, characterManagement, "Back", UI::ButtonState::ENABLED,
        [p_menuManager](){p_menuManager->RequestChangeActiveMenu("MainMenu");});
    AddButton(
        m_p_style,
        characterManagement,
        "Start",
        m_character_files.empty()? UI::ButtonState::DISABLED : UI::ButtonState::ENABLED,
        [p_menuManager](){p_menuManager->RequestChangeActiveMenu("ChooseWorld");});
    AddButton(
        m_p_style,
        characterManagement,
        "Remove",
        m_character_files.empty()? UI::ButtonState::DISABLED : UI::ButtonState::ENABLED,
        [this](){
            this->DeleteSelectedCharacter();
        }
    );
    AddButton(m_p_style, characterManagement, "New", UI::ButtonState::ENABLED,
        [p_menuManager](){p_menuManager->RequestChangeActiveMenu("CreateCharacter");});
    characterManagement.EmplaceChild<UI::Spacer>();

    if (!m_character_files.empty()) {
        SelectCharacter(m_selected_character_index);
    }
}

void Menu::ChooseCharacterMenu::Deactivate() {

    m_entity = ECS::Entity();
    m_character_preview_entity = ECS::Entity();
    m_character_files.clear();
    m_selected_character_index = 0U;
    m_p_character_name_element = nullptr;
}

void Menu::ChooseCharacterMenu::SelectCharacter(bool next) {

    // Determine which character to select
    if (next) {

        if (m_selected_character_index + 1 >= m_character_files.size()) {
            m_selected_character_index = 0U;
        }
        else {
            m_selected_character_index++;
        }
    }
    else {
        if (m_selected_character_index == 0U) {
            m_selected_character_index = m_character_files.size() - 1;
        }
        else {
            m_selected_character_index--;
        }
    }
    SelectCharacter(m_selected_character_index);
}

void Menu::ChooseCharacterMenu::SelectCharacter(size_t index) {

    // get the file name
    const std::string& filename = m_character_files.at(index);

    try {
        m_character_preview_entity = Character::LoadCharacterFromFile(filename);

        Components::CreatureInstance& creature = m_character_preview_entity.GetComponent<Components::CreatureInstance>();
        m_p_character_name_element->SetTextString(creature.m_name);

    } catch (std::exception& error) {
        m_character_preview_entity = ECS::Entity(m_p_engine->GetEcsRegistry());

        m_p_character_name_element->SetTextString(filename);
    }

    Components::Transform& transform = m_character_preview_entity.EmplaceComponent<Components::Transform>();
    transform.Translate(glm::vec3(0.0F, -1.0F, -1.0F));
}

void Menu::ChooseCharacterMenu::DeleteSelectedCharacter() {

    // Delete the file
    Character::DeleteCharacterFile(m_character_files.at(m_selected_character_index));

    // update the file list.
    std::vector<std::string> updatedCharacterFiles;
    updatedCharacterFiles.reserve(m_character_files.size());
    for (size_t index = 0U; index < m_character_files.size(); index++) {

        if (index != m_selected_character_index) {

            updatedCharacterFiles.push_back(std::move(m_character_files.at(index)));
        }
    }
    m_character_files = std::move(updatedCharacterFiles);

    if (m_character_files.empty()) {

        // Need to reset all buttons to disabled state. Maybe we can cheat and deactivate then reactivate menu
        Deactivate();
        Activate();
    }
    else {
        // choose the next character
        if (m_selected_character_index >= m_character_files.size()) {

            m_selected_character_index = m_character_files.size() - 1U;
        }

        // Select the character
        SelectCharacter(m_selected_character_index);
    }
}