#include "SettingsMenu.hpp"
#include "MenuManager.hpp"
#include "core/Engine.hpp"
#include "core/Logger.hpp"
#include "core/Settings.hpp"
#include "ecs/ECS.hpp"
#include "components/Canvas.hpp"
#include "graphics/Font.hpp"
#include "systems/RenderSystem.hpp"
#include "ui/ButtonStyle.hpp"
#include "ui/CheckBox.hpp"
#include "ui/CheckBoxStyle.hpp"
#include "ui/DropDown.hpp"
#include "ui/Spacer.hpp"
#include "ui/Switch.hpp"
#include "ui/VerticalLayout.hpp"
#include "ui/HorizontalLayout.hpp"
#include <glm/ext/vector_int2.hpp>
#include <sstream>
#include <string>

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

    UI::Switch& switcher = verticalLayout.EmplaceChild<UI::Switch>();
    m_p_submenu_switch = &switcher;

    settingsSelector.EmplaceChild<UI::Spacer>();

    BuildGameplaySettingsSubmenu(settingsSelector, switcher);
    BuildControlsSettingsSubmenu(settingsSelector, switcher);
    BuildGraphicsSettingsSubmenu(settingsSelector, switcher);
    BuildAudioSettingsSubmenu(settingsSelector, switcher);
    BuildAccessibilitySettingsSubmenu(settingsSelector, switcher);

    settingsSelector.EmplaceChild<UI::Spacer>();

    SelectButton(SubmenuID::GRAPHICS);

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

void Menu::SettingsMenu::SelectButton(SubmenuID submenuID) {

    const auto& submenuIter = m_switch_index.find(submenuID);
    if (submenuIter != m_switch_index.end()) {

        if (m_p_current_button != nullptr) {
            m_p_current_button->SetButtonState(UI::ButtonState::ENABLED);
        }

        Submenu& submenu = submenuIter->second;
        submenu.p_button->SetButtonState(UI::ButtonState::SELECTED);
        m_p_current_button = submenu.p_button;

        m_p_submenu_switch->SelectChild(submenu.switch_index);
    }
}

void Menu::SettingsMenu::BuildGameplaySettingsSubmenu(UI::HorizontalLayout& selectorPanel, UI::Switch& switcher) {

    Submenu submenu = {};
    submenu.switch_index = switcher.GetChildCount();
    switcher.EmplaceChild<UI::Spacer>();

    UI::Button& gameplay = selectorPanel.EmplaceChild<UI::Button>();
    gameplay.SetButtonStyle(m_p_style->GetButtonStyle("simple"))
        .SetText("Gameplay")
        .SetButtonState(UI::ButtonState::ENABLED)
        .SetOnClickCallback([this](){
            this->SelectButton(SubmenuID::GAMEPLAY);
        })
        .SetFixedSize({256.0F, 96.0F})
        .SetLayoutMode(UI::LayoutMode::FIXED);

    submenu.p_button = &gameplay;
    m_switch_index[SubmenuID::GAMEPLAY] = submenu;
}

void Menu::SettingsMenu::BuildControlsSettingsSubmenu(UI::HorizontalLayout& selectorPanel, UI::Switch& switcher) {

    Submenu submenu = {};
    submenu.switch_index = switcher.GetChildCount();
    switcher.EmplaceChild<UI::Spacer>();

    UI::Button& controls = selectorPanel.EmplaceChild<UI::Button>();
    controls.SetButtonStyle(m_p_style->GetButtonStyle("simple"))
        .SetText("Controls")
        .SetButtonState(UI::ButtonState::ENABLED)
        .SetOnClickCallback([this](){
            this->SelectButton(SubmenuID::CONTROLS);
        })
        .SetFixedSize({256.0F, 96.0F})
        .SetLayoutMode(UI::LayoutMode::FIXED);

    submenu.p_button = &controls;
    m_switch_index[SubmenuID::CONTROLS] = submenu;
}

void Menu::SettingsMenu::BuildGraphicsSettingsSubmenu(UI::HorizontalLayout& selectorPanel, UI::Switch& switcher) {

    Core::Settings& settings = m_p_engine->GetSettings();
    Systems::RenderSystem& renderSystem = m_p_engine->GetEcsRegistry().GetSystem<Systems::RenderSystem>();

    Submenu submenu = {};
    submenu.switch_index = switcher.GetChildCount();
    UI::HorizontalLayout& graphicsSubmenu = switcher.EmplaceChild<UI::HorizontalLayout>();

    // center
    graphicsSubmenu.EmplaceChild<UI::Spacer>();
    UI::VerticalLayout& optionsList = graphicsSubmenu.EmplaceChild<UI::VerticalLayout>();
    graphicsSubmenu.EmplaceChild<UI::Spacer>();

    // add some space to top of list
    optionsList.EmplaceChild<UI::Spacer>()
        .SetFixedSize({32.0F, 8.0F})
        .SetLayoutMode(UI::LayoutMode::FIXED);

    m_supported_resolutions = Core::GraphicsSettings::GetSupportedResolutions();

    AddDropdownMenu(
        optionsList,
        "Resolution",
        Core::GraphicsSettings::ResolutionsToStrings(m_supported_resolutions),
        Core::GraphicsSettings::FindClosestResolution(
            m_supported_resolutions,
            settings.GetGraphicsSettings().GetDisplayResolution()));

    AddToggleOption(
        optionsList,
        "Fullscreen",
        settings.GetGraphicsSettings().GetFullscreen(),
        [&settings,&renderSystem](bool state){
            settings.GetGraphicsSettings().SetFullscreen(state);
            renderSystem.SetFullscreen(state);
            settings.Save();
    });
    AddToggleOption(
        optionsList,
        "VSync",
        settings.GetGraphicsSettings().GetVsyncEnabled(),
        [&settings,&renderSystem](bool state){
            settings.GetGraphicsSettings().SetVsyncEnabled(state);
            renderSystem.SetVsync(state);
            settings.Save();
    });

    UI::Button& graphics = selectorPanel.EmplaceChild<UI::Button>();
    graphics.SetButtonStyle(m_p_style->GetButtonStyle("simple"))
        .SetText("Graphics")
        .SetButtonState(UI::ButtonState::ENABLED)
        .SetOnClickCallback([this](){
            this->SelectButton(SubmenuID::GRAPHICS);
        })
        .SetFixedSize({256.0F, 96.0F})
        .SetLayoutMode(UI::LayoutMode::FIXED);

    submenu.p_button = &graphics;
    m_switch_index[SubmenuID::GRAPHICS] = submenu;
}

void Menu::SettingsMenu::BuildAudioSettingsSubmenu(UI::HorizontalLayout& selectorPanel, UI::Switch& switcher) {

    Submenu submenu = {};
    submenu.switch_index = switcher.GetChildCount();
    switcher.EmplaceChild<UI::Spacer>();

    UI::Button& audio = selectorPanel.EmplaceChild<UI::Button>();
    audio.SetButtonStyle(m_p_style->GetButtonStyle("simple"))
        .SetText("Audio")
        .SetButtonState(UI::ButtonState::ENABLED)
        .SetOnClickCallback([this](){
            this->SelectButton(SubmenuID::AUDIO);
        })
        .SetFixedSize({256.0F, 96.0F})
        .SetLayoutMode(UI::LayoutMode::FIXED);

    submenu.p_button = &audio;
    m_switch_index[SubmenuID::AUDIO] = submenu;
}

void Menu::SettingsMenu::BuildAccessibilitySettingsSubmenu(UI::HorizontalLayout& selectorPanel, UI::Switch& switcher) {

    Submenu submenu = {};
    submenu.switch_index = switcher.GetChildCount();
    switcher.EmplaceChild<UI::Spacer>();

    UI::Button& accessibility = selectorPanel.EmplaceChild<UI::Button>();
    accessibility.SetButtonStyle(m_p_style->GetButtonStyle("simple"))
        .SetText("Accessibility")
        .SetButtonState(UI::ButtonState::ENABLED)
        .SetOnClickCallback([this](){
            this->SelectButton(SubmenuID::ACCESSIBILITY);
        })
        .SetFixedSize({256.0F, 96.0F})
        .SetLayoutMode(UI::LayoutMode::FIXED);

    submenu.p_button = &accessibility;
    m_switch_index[SubmenuID::ACCESSIBILITY] = submenu;
}


void Menu::SettingsMenu::AddToggleOption(
    UI::Element& parent, const std::string& textLabel, bool initialState, UI::CheckBoxStateCallback_t callback) {

    const std::shared_ptr<Graphics::Font>& p_font = m_p_style->GetFont("Default-UI");

    UI::HorizontalLayout& optionLayout = parent.EmplaceChild<UI::HorizontalLayout>();
    optionLayout.SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN);

    UI::TextElement& labelText = optionLayout.EmplaceChild<UI::TextElement>();
    labelText.SetFont(p_font)
        .SetText(p_font->CreateText(textLabel))
        .SetFixedSize({128.0F, 32.0F})
        .SetLayoutMode(UI::LayoutMode::FIXED)
        .SetOrigin({0.5F, 0.5F})
        .SetRelativePosition({0.5F, 0.5F});

    // Add full screen checkbox
    UI::CheckBox& fullscreen = optionLayout.EmplaceChild<UI::CheckBox>();
    fullscreen.SetStyle(m_p_style->GetCheckBoxStyle("simple"))
        .SetCheckBoxStateCallback(std::move(callback))
        .SetCheckBoxState(initialState? UI::CheckBoxState::ON : UI::CheckBoxState::OFF)
        .SetLayoutMode(UI::LayoutMode::FIXED)
        .SetFixedSize({32.0F, 32.0F});
}

void Menu::SettingsMenu::AddDropdownMenu(
    UI::Element& parent, const std::string& textLabel, std::vector<std::string> choices, uint32_t selected) {

    const std::shared_ptr<Graphics::Font>& p_font = m_p_style->GetFont("Default-UI");

    UI::HorizontalLayout& optionLayout = parent.EmplaceChild<UI::HorizontalLayout>();
    optionLayout.SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN);

    UI::TextElement& labelText = optionLayout.EmplaceChild<UI::TextElement>();
    labelText.SetFont(p_font)
        .SetText(p_font->CreateText(textLabel))
        .SetFixedSize({128.0F, 32.0F})
        .SetLayoutMode(UI::LayoutMode::FIXED)
        .SetOrigin({0.5F, 0.5F})
        .SetRelativePosition({0.5F, 0.5F});

    // Add drop down
    UI::DropDown& option = optionLayout.EmplaceChild<UI::DropDown>();
    option.SetStyle(m_p_style->GetDropDownStyle("simple"))
        .SetValueChangedCallback([](size_t selected){
            Core::Logger::Info("Selected " + std::to_string(selected));
        })
        .SetOptions(std::move(choices))
        .SelectOption(selected)
        .SetLayoutMode(UI::LayoutMode::FIXED)
        .SetFixedSize({160.0F, 96.0F});
}
