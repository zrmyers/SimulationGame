#include "SettingsMenu.hpp"
#include "MenuManager.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "components/Canvas.hpp"
#include "ui/Spacer.hpp"
#include "ui/VerticalLayout.hpp"
#include "ui/HorizontalLayout.hpp"

Menu::SettingsMenu::SettingsMenu(Core::Engine& engine, MenuManager& manager, std::shared_ptr<UI::Style> p_style)
    : m_p_engine(&engine)
    , m_p_manager(&manager)
    , m_p_style(std::move(p_style))
    , m_p_submenu_switch(nullptr)
    , m_p_current_button(nullptr) {
}

void Menu::SettingsMenu::Activate() {

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
        .SetRelativeSize({0.7F, 0.75F})
        .SetOrigin({0.5F, 0.5F})
        .SetLayoutMode(UI::LayoutMode::RELATIVE_TO_PARENT);

    UI::VerticalLayout& verticalLayout = nineslice.EmplaceChild<UI::VerticalLayout>();
    verticalLayout.SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN);

    UI::HorizontalLayout& settingsSelector = verticalLayout.EmplaceChild<UI::HorizontalLayout>();
    settingsSelector.SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN)
        .SetOrigin({0.5F, 0.5F})
        .SetRelativePosition({0.5F, 0.5F});

    settingsSelector.EmplaceChild<UI::Spacer>();

    UI::Button& gameplay = settingsSelector.EmplaceChild<UI::Button>();
    gameplay.SetButtonStyle(m_p_style->GetButtonStyle("simple"))
        .SetText("Gameplay")
        .SetButtonState(UI::ButtonState::ENABLED)
        .SetOnClickCallback([&gameplay, this](){
            this->SelectButton(gameplay);
        })
        .SetFixedSize({256.0F, 96.0F})
        .SetLayoutMode(UI::LayoutMode::FIXED);

    UI::Button& controls = settingsSelector.EmplaceChild<UI::Button>();
    controls.SetButtonStyle(m_p_style->GetButtonStyle("simple"))
        .SetText("Controls")
        .SetButtonState(UI::ButtonState::ENABLED)
        .SetOnClickCallback([&controls, this](){
            this->SelectButton(controls);
        })
        .SetFixedSize({256.0F, 96.0F})
        .SetLayoutMode(UI::LayoutMode::FIXED);

    UI::Button& graphics = settingsSelector.EmplaceChild<UI::Button>();
    graphics.SetButtonStyle(m_p_style->GetButtonStyle("simple"))
        .SetText("Graphics")
        .SetButtonState(UI::ButtonState::ENABLED)
        .SetOnClickCallback([&graphics, this](){
            this->SelectButton(graphics);
        })
        .SetFixedSize({256.0F, 96.0F})
        .SetLayoutMode(UI::LayoutMode::FIXED);

    UI::Button& audio = settingsSelector.EmplaceChild<UI::Button>();
    audio.SetButtonStyle(m_p_style->GetButtonStyle("simple"))
        .SetText("Audio")
        .SetButtonState(UI::ButtonState::ENABLED)
        .SetOnClickCallback([&audio, this](){
            this->SelectButton(audio);
        })
        .SetFixedSize({256.0F, 96.0F})
        .SetLayoutMode(UI::LayoutMode::FIXED);

    UI::Button& accessibility = settingsSelector.EmplaceChild<UI::Button>();
    accessibility.SetButtonStyle(m_p_style->GetButtonStyle("simple"))
        .SetText("Accessibility")
        .SetButtonState(UI::ButtonState::ENABLED)
        .SetOnClickCallback([&accessibility, this](){
            this->SelectButton(accessibility);
        })
        .SetFixedSize({256.0F, 96.0F})
        .SetLayoutMode(UI::LayoutMode::FIXED);

    settingsSelector.EmplaceChild<UI::Spacer>();

    SelectButton(graphics);

    verticalLayout.EmplaceChild<UI::Spacer>();

    UI::HorizontalLayout& navBar = verticalLayout.EmplaceChild<UI::HorizontalLayout>();
    navBar.SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN);

    navBar.EmplaceChild<UI::Spacer>();

    UI::Button& backButton = navBar.EmplaceChild<UI::Button>();
    backButton.SetButtonStyle(m_p_style->GetButtonStyle("simple"))
        .SetText("Back")
        .SetButtonState(UI::ButtonState::ENABLED)
        .SetOnClickCallback([p_menuManager](){
            p_menuManager->RequestChangeActiveMenu("MainMenu");
        })
        .SetFixedSize({256.0F, 96.0F})
        .SetLayoutMode(UI::LayoutMode::FIXED);

    navBar.EmplaceChild<UI::Spacer>();
}

void Menu::SettingsMenu::Deactivate() {

    m_entity = ECS::Entity();
}

void Menu::SettingsMenu::SelectButton(UI::Button& newButton) {

    if (m_p_current_button != nullptr) {
        m_p_current_button->SetButtonState(UI::ButtonState::ENABLED);
    }

    newButton.SetButtonState(UI::ButtonState::SELECTED);
    m_p_current_button = &newButton;
}