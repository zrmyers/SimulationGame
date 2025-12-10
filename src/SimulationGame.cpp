#include "SimulationGame.hpp"
#include "components/Camera.hpp"
#include "components/Canvas.hpp"
#include "components/Renderable.hpp"
#include "components/Text.hpp"
#include "components/Transform.hpp"
#include "core/AssetLoader.hpp"
#include "core/IGame.hpp"
#include "core/Logger.hpp"
#include "ecs/ECS.hpp"
#include "sdl/TTF.hpp"
#include "systems/GuiSystem.hpp"
#include "systems/RenderSystem.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

#include "systems/TextSystem.hpp"
#include "ui/UI.hpp"

SimulationGame::SimulationGame(Core::Engine& engine)
    : Core::IGame(engine) {
    Core::Logger::Info("Initializing Game.");
    ECS::Registry& registry = engine.GetEcsRegistry();
    Systems::RenderSystem& renderer = registry.GetSystem<Systems::RenderSystem>();
    Systems::TextSystem& textRenderer = registry.GetSystem<Systems::TextSystem>();
    Systems::GuiSystem& guiSystem = registry.GetSystem<Systems::GuiSystem>();

    m_p_font = textRenderer.CreateFont(
        "Oblegg-Regular.otf", 50.0F, true, TTF_HORIZONTAL_ALIGN_CENTER);

    // setup text object
    m_text_entity = ECS::Entity(registry);
    m_text_entity.EmplaceComponent<Components::Transform>();
    Components::Text& text = m_text_entity.EmplaceComponent<Components::Text>();
    text.m_p_font = m_p_font;
    text.m_color = glm::vec4(0.0F, 1.0F, 0.0F, 1.0F);
    text.m_p_text = m_p_font->CreateText("Hello\n12345");
    text.m_layer = Components::RenderLayer::LAYER_3D_OPAQUE;

    // setup camera object
    m_camera_entity = ECS::Entity(registry);
    m_camera_entity.EmplaceComponent<Components::Camera>();

    guiSystem.SetCursor("/cursor.png", true);

    InitializeGUI();
}

void SimulationGame::Update() {

    float deltaTimeSec = GetEngine().GetDeltaTimeSec();
    ECS::Registry& registry = GetEngine().GetEcsRegistry();
    std::stringstream msgStream;
    msgStream << std::fixed << std::setprecision(2);
    msgStream << "FPS: " << 1.0F / deltaTimeSec;

    Components::Text& text = m_text_entity.GetComponent<Components::Text>();
    text.m_p_text->SetString(msgStream.str());

    if (text.m_p_text != nullptr) {
        int textWidth = 0;
        int textHeight = 0;
        text.m_p_text->GetSize(textWidth, textHeight);

        m_rotateAngle += glm::pi<float>()/2.0F * deltaTimeSec;
        Components::Transform& transform = m_text_entity.GetComponent<Components::Transform>();

        // text pipeline
        transform
            .Set(glm::mat4(1.0F))
            .Translate(glm::vec3(0.0F, -0.0F, -80.0F))
            .Scale(glm::vec3(0.3F, 0.3F, 0.3F))
            .Rotate(m_rotateAngle, glm::vec3(0.0F, 1.0F, 0.0F));
    }

}

void SimulationGame::InitializeGUI() {

    Core::Engine& engine = GetEngine();
    Core::AssetLoader& assetLoader = GetEngine().GetAssetLoader();
    ECS::Registry& registry = GetEngine().GetEcsRegistry();
    Systems::RenderSystem& renderSystem = registry.GetSystem<Systems::RenderSystem>();
    Systems::TextSystem& textSystem = registry.GetSystem<Systems::TextSystem>();

    m_gui_entity = ECS::Entity(registry);

    auto& canvas = m_gui_entity.EmplaceComponent<Components::Canvas>();

    canvas.SetRenderMode(Components::Canvas::RenderMode::SCREEN);

    UI::Style uiStyle = UI::Style::Load(GetEngine(), "ui-style.json");
    std::shared_ptr<UI::NineSliceStyle> boxStyle = uiStyle.GetNineSliceStyle("box-style");

    UI::NineSlice& nineslice = canvas.EmplaceChild<UI::NineSlice>();
    nineslice
        .SetStyle(boxStyle)
        .SetRelativePosition({0.5, 0.5})
        .SetRelativeSize({0.5F, 0.5F})
        .SetOrigin({0.5F, 0.5F});

    UI::VerticalLayout& verticalLayout = nineslice.EmplaceChild<UI::VerticalLayout>();
    verticalLayout.SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN);

    UI::Button& startButton = verticalLayout.EmplaceChild<UI::Button>();
    startButton.SetButtonStyle(uiStyle.GetButtonStyle("simple"))
        .SetText("Start")
        .SetButtonState(UI::ButtonState::DISABLED);
    UI::Button& settingsButton = verticalLayout.EmplaceChild<UI::Button>();
    settingsButton.SetButtonStyle(uiStyle.GetButtonStyle("simple"))
        .SetText("Settings")
        .SetButtonState(UI::ButtonState::ENABLED);
    UI::Button& quitButton = verticalLayout.EmplaceChild<UI::Button>();
    quitButton.SetButtonStyle(uiStyle.GetButtonStyle("simple"))
        .SetText("Quit")
        .SetButtonState(UI::ButtonState::ENABLED)
        .SetOnClickCallback([&engine](){
            engine.RequestShutdown();
        });
}