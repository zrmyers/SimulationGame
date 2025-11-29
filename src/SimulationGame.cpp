#include "SimulationGame.hpp"
#include "components/Camera.hpp"
#include "components/Renderable.hpp"
#include "components/Sprite.hpp"
#include "components/Text.hpp"
#include "components/Transform.hpp"
#include "core/AssetLoader.hpp"
#include "core/IGame.hpp"
#include "core/Logger.hpp"
#include "ecs/ECS.hpp"
#include "sdl/SDL.hpp"
#include "systems/RenderSystem.hpp"
#include "graphics/ShaderCross.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <array>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <iomanip>
#include <sstream>
#include <string>

#include "glm/mat4x4.hpp"
#include "systems/TextSystem.hpp"

SimulationGame::SimulationGame(Core::Engine& engine)
    : Core::IGame(engine) {
    Core::Logger::Info("Initializing Game.");
    ECS::Registry& registry = engine.GetEcsRegistry();
    Core::AssetLoader& assetLoader = engine.GetAssetLoader();
    Systems::RenderSystem& renderer = registry.GetSystem<Systems::RenderSystem>();
    Systems::TextSystem& textRenderer = registry.GetSystem<Systems::TextSystem>();

    m_font = textRenderer.CreateFont(assetLoader.GetFontDir() + "/Oblegg-Regular.otf", 50.0F); // NOLINT
    m_font->SetHorizontalAlignment(TTF_HORIZONTAL_ALIGN_CENTER);
    m_font->SetSDF(true);

    // setup text object
    m_text_entity = ECS::Entity(registry);
    m_text_entity.EmplaceComponent<Components::Transform>();
    Components::Text& text = m_text_entity.EmplaceComponent<Components::Text>();
    text.m_p_font = m_font;
    text.m_color = glm::vec4(0.0F, 1.0F, 0.0F, 1.0F);
    text.m_string = "Hello World\n1234";
    text.m_layer = Components::RenderLayer::LAYER_3D_OPAQUE;

    // setup camera object
    m_camera_entity = ECS::Entity(registry);
    m_camera_entity.EmplaceComponent<Components::Camera>();

    // setup cursor object
    m_cursor_entity = ECS::Entity(registry);
    Components::Sprite& cursor = m_cursor_entity.EmplaceComponent<Components::Sprite>();
    cursor.color = glm::vec4(1.0F,1.0F, 1.0F, 1.0F);
    cursor.topLeftUV = {0.0F, 0.0F};
    cursor.bottomRightUV = {1.0F,1.0F};

    // loading a texture is still lot of work, but getting there.
    SDL::Image image(assetLoader.GetImageDir() + "/cursor.png");
    SDL_GPUTextureCreateInfo textureInfo = {};
    textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.width = image.GetWidth();
    textureInfo.height = image.GetHeight();
    textureInfo.layer_count_or_depth = 1;
    textureInfo.num_levels = 1;
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    cursor.texture = renderer.CreateTexture(textureInfo);
    cursor.texture.SetName("Cursor");
    cursor.layer = Components::RenderLayer::LAYER_GUI;

    // texture transfer
    Systems::RenderSystem::TransferRequest request = {};
    request.cycle = false;
    request.type = Systems::RenderSystem::RequestType::UPLOAD_TO_TEXTURE;
    SDL_GPUTextureRegion& region = request.data.texture;
    region.texture = cursor.texture.Get();
    region.w = image.GetWidth();
    region.h = image.GetHeight();
    region.d = 1;
    request.p_src = image.GetPixels();

    renderer.UploadDataToBuffer({request});

    Components::Transform& transform = m_cursor_entity.EmplaceComponent<Components::Transform>();
    transform
        .Scale({static_cast<float>(region.w)/1024.0F, static_cast<float>(region.h)/768.0F, 1.0F})  // scale cursor by half.
        .Rotate(glm::pi<float>()/5.0F, glm::vec3(0.0F, 0.0F, 1.0F));
}

void SimulationGame::Update() {

    float deltaTimeSec = GetEngine().GetDeltaTimeSec();
    ECS::Registry& registry = GetEngine().GetEcsRegistry();
    std::stringstream msgStream;
    msgStream << std::fixed << std::setprecision(2);
    msgStream << "FPS: " << 1.0F / deltaTimeSec;

    Components::Text& text = m_text_entity.GetComponent<Components::Text>();
    text.m_string = msgStream.str();

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

