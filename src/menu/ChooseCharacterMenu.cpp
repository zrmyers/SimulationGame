#include "ChooseCharacterMenu.hpp"
#include "MenuUtilities.hpp"
#include "components/Canvas.hpp"
#include "ecs/ECS.hpp"
#include "ui/Button.hpp"
#include "ui/ButtonStyle.hpp"
#include "ui/HorizontalLayout.hpp"
#include "ui/Spacer.hpp"
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

    Components::Canvas& canvas = m_entity.EmplaceComponent<Components::Canvas>();
    canvas.SetRenderMode(Components::Canvas::RenderMode::SCREEN);
    // populate canvas with ui elements

    UI::VerticalLayout& vertical = canvas.EmplaceChild<UI::VerticalLayout>();

    vertical.EmplaceChild<UI::Spacer>();  // top of screen should be empty space

    // This is bar for choosing between various saved characters, if they exist
    UI::HorizontalLayout& characterRotary = vertical.EmplaceChild<UI::HorizontalLayout>();
    characterRotary.EmplaceChild<UI::Spacer>();
    AddButton(m_p_style, characterRotary, "<", UI::ButtonState::DISABLED);
    AddButton(m_p_style, characterRotary, ">", UI::ButtonState::DISABLED);
    characterRotary.EmplaceChild<UI::Spacer>();

    // menu navigation and character management bar
    UI::HorizontalLayout& characterManagement = vertical.EmplaceChild<UI::HorizontalLayout>();
    characterManagement.EmplaceChild<UI::Spacer>();
    AddButton(m_p_style, characterManagement, "Back", UI::ButtonState::ENABLED,
        [p_menuManager](){p_menuManager->RequestChangeActiveMenu("MainMenu");});
    AddButton(m_p_style, characterManagement, "Start", UI::ButtonState::DISABLED);
    AddButton(m_p_style, characterManagement, "Remove", UI::ButtonState::DISABLED);
    AddButton(m_p_style, characterManagement, "New", UI::ButtonState::DISABLED);
    characterManagement.EmplaceChild<UI::Spacer>();
}

void Menu::ChooseCharacterMenu::Deactivate() {

    m_entity = ECS::Entity();
}