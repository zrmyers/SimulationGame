#include "SimulationGame.hpp"
#include "components/Camera.hpp"
#include "components/Canvas.hpp"
#include "components/Renderable.hpp"
#include "components/Sprite.hpp"
#include "components/Text.hpp"
#include "components/Transform.hpp"
#include "core/AssetLoader.hpp"
#include "core/IGame.hpp"
#include "core/Logger.hpp"
#include "ecs/ECS.hpp"
#include "graphics/Texture2D.hpp"
#include "sdl/SDL.hpp"
#include "sdl/TTF.hpp"
#include "systems/GuiSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "graphics/ShaderCross.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <array>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "glm/mat4x4.hpp"
#include "systems/TextSystem.hpp"
#include "ui/UI.hpp"

SimulationGame::SimulationGame(Core::Engine& engine)
    : Core::IGame(engine) {
    Core::Logger::Info("Initializing Game.");
    ECS::Registry& registry = engine.GetEcsRegistry();
    Core::AssetLoader& assetLoader = engine.GetAssetLoader();
    Systems::RenderSystem& renderer = registry.GetSystem<Systems::RenderSystem>();
    Systems::TextSystem& textRenderer = registry.GetSystem<Systems::TextSystem>();
    Systems::GuiSystem& guiSystem = registry.GetSystem<Systems::GuiSystem>();

    m_font = textRenderer.CreateFont(assetLoader.GetFontDir() + "/Oblegg-Regular.otf", 50.0F); // NOLINT
    m_font->SetHorizontalAlignment(TTF_HORIZONTAL_ALIGN_CENTER);
    m_font->SetSDF(true);

    // setup text object
    m_text_entity = ECS::Entity(registry);
    m_text_entity.EmplaceComponent<Components::Transform>();
    Components::Text& text = m_text_entity.EmplaceComponent<Components::Text>();
    text.m_p_font = m_font;
    text.m_color = glm::vec4(0.0F, 1.0F, 0.0F, 1.0F);
    text.m_p_text = textRenderer.CreateText(m_font, "Hello\n12345");
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

    Core::AssetLoader& assetLoader = GetEngine().GetAssetLoader();
    ECS::Registry& registry = GetEngine().GetEcsRegistry();
    Systems::RenderSystem& renderSystem = registry.GetSystem<Systems::RenderSystem>();
    Systems::TextSystem& textSystem = registry.GetSystem<Systems::TextSystem>();

    m_gui_entity = ECS::Entity(registry);

    auto& canvas = m_gui_entity.EmplaceComponent<Components::Canvas>();

    canvas.SetRenderMode(Components::Canvas::RenderMode::SCREEN);

    SDL::Image image(assetLoader.GetImageDir() + "/nineslice-top-right.png");

    SDL_GPUSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    samplerCreateInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerCreateInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.enable_anisotropy = true;
    samplerCreateInfo.max_anisotropy = 16; // NOLINT

    std::shared_ptr<SDL::GpuSampler> p_sampler = std::make_shared<SDL::GpuSampler>(renderSystem.CreateSampler(samplerCreateInfo));

    std::shared_ptr<Graphics::Texture2D> p_texture =
         std::make_shared<Graphics::Texture2D>(GetEngine(), p_sampler, image.GetWidth(), image.GetHeight(), false);

    p_texture->LoadImageData(image);

    UI::HorizontalLayout& hzLayout = canvas.EmplaceChild<UI::HorizontalLayout>();

    std::shared_ptr<SDL::TTF::Font> p_uiFont = textSystem.CreateFont(assetLoader.GetFontDir() + "/Oblegg-Regular.otf", 32.0F);
    p_uiFont->SetSDF(true);
    p_uiFont->SetHorizontalAlignment(TTF_HORIZONTAL_ALIGN_LEFT);

    UI::Element& vertLayout = hzLayout.EmplaceChild<UI::VerticalLayout>();
    UI::TextElement& textelement1 = vertLayout.EmplaceChild<UI::TextElement>();
    textelement1
        .SetFont(p_uiFont)
        .SetText(textSystem.CreateText(p_uiFont, "ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz!"))
        .SetFixedSize(textelement1.GetTextSize())
        //.SetOrigin({-0.5F, -0.5F})
        .SetLayoutMode(UI::LayoutMode::FIXED);
    UI::TextElement& textelement2 = vertLayout.EmplaceChild<UI::TextElement>();
    textelement2
        .SetFont(p_uiFont)
        .SetText(textSystem.CreateText(p_uiFont, "Some more text to test the text!"))
        .SetFixedSize(textelement2.GetTextSize())
        //.SetOrigin({-0.5F, -0.5F})
        .SetLayoutMode(UI::LayoutMode::FIXED);

    UI::ImageElement& imageElement = hzLayout.EmplaceChild<UI::ImageElement>();
    imageElement
        .SetTexture(p_texture)
        .SetFixedSize({p_texture->GetWidth(), p_texture->GetHeight()})
        .SetOrigin({0.5F, 0.5F})
        .SetLayoutMode(UI::LayoutMode::RELATIVE_TO_PARENT);
}