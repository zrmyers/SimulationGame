#include "SimulationGame.hpp"
#include "components/Camera.hpp"
#include "core/IGame.hpp"
#include "core/Logger.hpp"
#include "ecs/ECS.hpp"
#include "menu/ChooseCharacterMenu.hpp"
#include "menu/CreateCharacterMenu.hpp"
#include "menu/ChooseWorldMenu.hpp"
#include "menu/CreateWorldMenu.hpp"
#include "menu/MainMenu.hpp"
#include "menu/SettingsMenu.hpp"
#include "systems/GuiSystem.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <memory>
#include <string>

SimulationGame::SimulationGame(Core::Engine& engine)
    : Core::IGame(engine) {
    Core::Logger::Info("Initializing Game.");
    ECS::Registry& registry = engine.GetEcsRegistry();
    Systems::GuiSystem& guiSystem = registry.GetSystem<Systems::GuiSystem>();

    // setup camera object
    m_camera_entity = ECS::Entity(registry);
    m_camera_entity.EmplaceComponent<Components::Camera>();

    guiSystem.SetCursor("/cursor.png", true);

    engine.AddNameGenerator("Regions", "names.json");
    InitializeGUI();
}

void SimulationGame::Update() {
    m_menu_manager.Update();
}

void SimulationGame::InitializeGUI() {

    std::shared_ptr<UI::Style> uiStyle = std::make_shared<UI::Style>(UI::Style::Load(GetEngine(), "ui-style.json"));
    std::unique_ptr<Menu::MainMenu> p_menu =
        std::make_unique<Menu::MainMenu>(GetEngine(), m_menu_manager, uiStyle);
    std::unique_ptr<Menu::SettingsMenu> p_settings =
        std::make_unique<Menu::SettingsMenu>(GetEngine(), m_menu_manager, uiStyle);
    std::unique_ptr<Menu::ChooseCharacterMenu> p_character
        = std::make_unique<Menu::ChooseCharacterMenu>(GetEngine(), m_menu_manager, uiStyle);
    std::unique_ptr<Menu::CreateCharacterMenu> p_createCharacter
        = std::make_unique<Menu::CreateCharacterMenu>(GetEngine(), m_menu_manager, uiStyle);
    std::unique_ptr<Menu::ChooseWorldMenu> p_chooseWorld
        = std::make_unique<Menu::ChooseWorldMenu>(GetEngine(), m_menu_manager, uiStyle);
    std::unique_ptr<Menu::CreateWorldMenu> p_createWorld
        = std::make_unique<Menu::CreateWorldMenu>(GetEngine(), m_menu_manager, uiStyle);
    m_menu_manager.AddMenu("MainMenu", std::move(p_menu));
    m_menu_manager.AddMenu("Settings", std::move(p_settings));
    m_menu_manager.AddMenu("ChooseCharacter", std::move(p_character));
    m_menu_manager.AddMenu("ChooseWorld", std::move(p_chooseWorld));
    m_menu_manager.AddMenu("CreateWorld", std::move(p_createWorld));
    m_menu_manager.AddMenu("CreateCharacter", std::move(p_createCharacter));

    m_menu_manager.RequestChangeActiveMenu("MainMenu");
}