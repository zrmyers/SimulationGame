#include "MainMenu.hpp"
#include "MenuManager.hpp"
#include "MenuUtilities.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "components/Canvas.hpp"
#include "ui/ButtonStyle.hpp"
#include "ui/NineSlice.hpp"
#include "ui/VerticalLayout.hpp"
#include "ui/Button.hpp"

Menu::MainMenu::MainMenu(Core::Engine& engine, MenuManager& manager, std::shared_ptr<UI::Style> p_style)
    : m_p_engine(&engine)
    , m_p_manager(&manager)
    , m_p_style(std::move(p_style)) {
}

void Menu::MainMenu::Activate() {

    Core::Engine* p_engine = m_p_engine;
    MenuManager* p_menuManager = m_p_manager;
    ECS::Registry& registry = m_p_engine->GetEcsRegistry();

    m_entity = ECS::Entity(registry);

    auto& canvas = m_entity.EmplaceComponent<Components::Canvas>();

    canvas.SetRenderMode(Components::Canvas::RenderMode::SCREEN);

    std::shared_ptr<UI::NineSliceStyle> boxStyle = m_p_style->GetNineSliceStyle("box-style");

    UI::NineSlice& nineslice = canvas.EmplaceChild<UI::NineSlice>();
    nineslice
        .SetStyle(boxStyle)
        .SetRelativePosition({0.5, 0.5})
        .SetRelativeSize({0.5F, 0.5F})
        .SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN)
        .SetOrigin({0.5F, 0.5F});

    UI::VerticalLayout& verticalLayout = nineslice.EmplaceChild<UI::VerticalLayout>();
    verticalLayout.SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN);

    AddButton(m_p_style, verticalLayout, "Start", UI::ButtonState::ENABLED,
        [p_menuManager](){p_menuManager->RequestChangeActiveMenu("ChooseCharacter");});
    AddButton(m_p_style, verticalLayout, "Settings", UI::ButtonState::ENABLED,
        [p_menuManager](){p_menuManager->RequestChangeActiveMenu("Settings");});
    AddButton(m_p_style, verticalLayout, "Quit", UI::ButtonState::ENABLED,
        [p_engine](){p_engine->RequestShutdown();});
}

void Menu::MainMenu::Deactivate() {

    m_entity = ECS::Entity();
}