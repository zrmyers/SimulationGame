#include "CreateCharacterMenu.hpp"
#include "ChooseCharacterMenu.hpp"
#include "MenuUtilities.hpp"
#include "character/Character.hpp"
#include "components/Canvas.hpp"
#include "ecs/ECS.hpp"
#include "ui/Button.hpp"
#include "ui/ButtonStyle.hpp"
#include "ui/HorizontalLayout.hpp"
#include "ui/Spacer.hpp"
#include "ui/VerticalLayout.hpp"
#include <memory>

Menu::CreateCharacterMenu::CreateCharacterMenu(Core::Engine& engine, MenuManager& manager, std::shared_ptr<UI::Style> p_style)
    : m_p_engine(&engine)
    , m_p_manager(&manager)
    , m_p_style(std::move(p_style)) {
}

void Menu::CreateCharacterMenu::Activate() {

    Core::Engine* p_engine = m_p_engine;
    MenuManager* p_menuManager = m_p_manager;
    ECS::Registry& registry = m_p_engine->GetEcsRegistry();

    m_entity = ECS::Entity(registry);

    // Instantiate the new Character
    m_p_new_character = Character::Character::CreateDefault(*m_p_engine);

    // Draw the character to screen

    Components::Canvas& canvas = m_entity.EmplaceComponent<Components::Canvas>();
    canvas.SetRenderMode(Components::Canvas::RenderMode::SCREEN);
    // populate canvas with ui elements

    UI::HorizontalLayout& horizontal = canvas.EmplaceChild<UI::HorizontalLayout>();

    horizontal.EmplaceChild<UI::Spacer>();
    UI::VerticalLayout& vertical = horizontal.EmplaceChild<UI::VerticalLayout>();
    horizontal.EmplaceChild<UI::Spacer>();

    vertical.EmplaceChild<UI::Spacer>();  // top of screen should be empty space

    // menu navigation and character management bar
    UI::HorizontalLayout& characterManagement = vertical.EmplaceChild<UI::HorizontalLayout>();
    characterManagement.EmplaceChild<UI::Spacer>();
    AddButton(m_p_style, characterManagement, "Cancel", UI::ButtonState::ENABLED,
        [p_menuManager](){p_menuManager->RequestChangeActiveMenu("ChooseCharacter");});
    AddButton(m_p_style, characterManagement, "Done", UI::ButtonState::DISABLED,
        [p_menuManager](){
            // save character to file
            // return to previous screen
            p_menuManager->RequestChangeActiveMenu("ChooseCharacter");
        });
    characterManagement.EmplaceChild<UI::Spacer>();
}

void Menu::CreateCharacterMenu::Deactivate() {

    m_entity = ECS::Entity();

    m_p_new_character = nullptr;
}