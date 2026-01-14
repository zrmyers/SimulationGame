/**
 * @file ChooseWorldMenu.cpp
 * @brief Menu for selecting which world to play in.
 */

#include "ChooseWorldMenu.hpp"
#include "MenuUtilities.hpp"
#include "components/Canvas.hpp"
#include "ui/Button.hpp"
#include "ui/ButtonStyle.hpp"
#include "ui/HorizontalLayout.hpp"
#include "ui/Spacer.hpp"
#include "ui/TextElement.hpp"
#include "ui/VerticalLayout.hpp"
#include "core/Logger.hpp"

namespace Menu {
ChooseWorldMenu::ChooseWorldMenu(Core::Engine& engine, MenuManager& manager, std::shared_ptr<UI::Style> p_style)
    : m_p_engine(&engine)
    , m_p_manager(&manager)
    , m_p_style(std::move(p_style)) {
}

void ChooseWorldMenu::Activate() {

    ECS::Registry& registry = m_p_engine->GetEcsRegistry();
    m_entity = ECS::Entity(registry);

    m_selected_world_index = 0U;

    Components::Canvas& canvas = m_entity.EmplaceComponent<Components::Canvas>();
    canvas.SetRenderMode(Components::Canvas::RenderMode::SCREEN);

    UI::VerticalLayout& vertical = canvas.EmplaceChild<UI::VerticalLayout>();

    vertical.EmplaceChild<UI::Spacer>();

    // This is bar for choosing between various saved characters, if they exist
    UI::HorizontalLayout& worldRotary = vertical.EmplaceChild<UI::HorizontalLayout>();
    worldRotary.EmplaceChild<UI::Spacer>();
    AddButton(
        m_p_style,
        worldRotary,
        "<",
        m_worlds.empty()? UI::ButtonState::DISABLED : UI::ButtonState::ENABLED,
        [this](){ this->SelectWorld(false); });
    UI::TextElement& worldName = AddTextElement(
        m_p_style,
        worldRotary,
        "None",
        glm::vec4(1.0F, 1.0F, 1.0F, 1.0F),
        20U);
    m_p_world_name_element = &worldName;
    AddButton(
        m_p_style,
        worldRotary,
        ">",
        m_worlds.empty()? UI::ButtonState::DISABLED : UI::ButtonState::ENABLED,
        [this](){ this->SelectWorld(true); });
    worldRotary.EmplaceChild<UI::Spacer>();

    UI::HorizontalLayout& bottomBar = vertical.EmplaceChild<UI::HorizontalLayout>();
    bottomBar.EmplaceChild<UI::Spacer>();

    AddButton(m_p_style, bottomBar, "Back", UI::ButtonState::ENABLED,
        [this](){ m_p_manager->RequestChangeActiveMenu("ChooseCharacter"); });

    AddButton(m_p_style, bottomBar, "Start", UI::ButtonState::DISABLED,
        [this](){
            Core::Logger::Info("Play requested for world: " + m_worlds.at(m_selected_world_index));
            // Placeholder: switch to MainMenu (or start game) — replace with actual game start logic.
            m_p_manager->RequestChangeActiveMenu("MainMenu");
        });
    AddButton(m_p_style, bottomBar, "Remove", m_worlds.empty()? UI::ButtonState::DISABLED : UI::ButtonState::ENABLED,
        [this](){
            this->DeleteSelectedWorld();
        });
    AddButton(m_p_style, bottomBar, "New", UI::ButtonState::ENABLED,
        [this](){
            m_p_manager->RequestChangeActiveMenu("CreateWorld");
        });

    bottomBar.EmplaceChild<UI::Spacer>();
}

void ChooseWorldMenu::Deactivate() {
    m_entity = ECS::Entity();
    m_worlds.clear();
    m_selected_world_index = 0U;
    m_p_world_name_element = nullptr;
}


void ChooseWorldMenu::SelectWorld(bool next) {

    (void) next;
}


void ChooseWorldMenu::DeleteSelectedWorld() {


}

}