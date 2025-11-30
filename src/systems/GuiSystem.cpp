#include "GuiSystem.hpp"
#include "RenderSystem.hpp"
#include "components/Renderable.hpp"
#include "core/AssetLoader.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"

#include "components/Sprite.hpp"
#include "components/Transform.hpp"
#include "sdl/SDL.hpp"
#include <SDL3/SDL_events.h>
#include <glm/fwd.hpp>
#include <iostream>
#include <string>
#include <vector>

Systems::GuiSystem::GuiSystem(Core::Engine& engine)
    : ECS::System(engine)
    , m_window_size_px(engine.GetEcsRegistry().GetSystem<Systems::RenderSystem>().GetWindowSize())
    , m_cursor_size_px(0.0F)
    , m_cursor_pos_px(0.0F) {

}

void Systems::GuiSystem::Update() {

    // process events
    const std::vector<SDL_Event>& events = GetEngine().GetEvents();

    for (const SDL_Event& event : events) {

        switch (event.type) {

            case SDL_EVENT_MOUSE_MOTION:
                HandleMouseMotion(event.motion);
                break;

            default:
                break;
        }
    }

}

void Systems::GuiSystem::NotifyEntityDestroyed(ECS::EntityID_t entityID) {

    // clean up related sprites and text
}

void Systems::GuiSystem::SetCursorVisible(bool enable) {

    if (m_cursor_entity.IsValid() && m_cursor_entity.HasComponent<Components::Sprite>()) {

        auto& sprite = m_cursor_entity.GetComponent<Components::Sprite>();
        sprite.layer = (enable)? Components::RenderLayer::LAYER_GUI : Components::RenderLayer::LAYER_NONE;
    }
}

bool Systems::GuiSystem::GetCursorVisible() const {

    bool isVisible = false;
    if (m_cursor_entity.IsValid() && m_cursor_entity.HasComponent<Components::Sprite>()) {

        const auto& sprite = m_cursor_entity.GetComponent<Components::Sprite>();
        isVisible = (sprite.layer != Components::RenderLayer::LAYER_NONE);
    }

    return isVisible;
}

void Systems::GuiSystem::UpdateCursor() {

    Components::Transform& transform = m_cursor_entity.FindOrEmplaceComponent<Components::Transform>();
    glm::vec2 center = m_window_size_px / 2.0F;
    glm::vec2 translate = center - m_cursor_pos_px;
    // translate needs to be changed from pixel coordinate to screen coordinate. [0, resX] -> [-1.0, 1.0]
    translate /= m_window_size_px;
    translate *= 2.0F;
    translate.x *= -1.0F;

    transform
        .Set(glm::mat4(1.0F))
        .Translate({translate, 0.0F})
        .Scale({m_cursor_size_px / m_window_size_px, 1.0F})
        .Rotate(glm::pi<float>()/5.0F, glm::vec3(0.0F, 0.0F, 1.0F))
        .Translate({0.0F, -0.5F, 0.0F}); // set origin at x=0.5F, y=0.0F
}

void Systems::GuiSystem::HandleMouseMotion(const SDL_MouseMotionEvent& event) {

    m_cursor_pos_px.x = event.x;
    m_cursor_pos_px.y = event.y;

    if (GetCursorVisible()) {
        UpdateCursor();
    }
}

void Systems::GuiSystem::SetCursor(const std::string& image_filename, bool visible) {

    Core::Engine& engine = GetEngine();
    Core::AssetLoader& assetLoader = engine.GetAssetLoader();
    ECS::Registry& registry = engine.GetEcsRegistry();
    Systems::RenderSystem& renderer = registry.GetSystem<Systems::RenderSystem>();

    // setup cursor object
    m_cursor_entity = ECS::Entity(registry);
    Components::Sprite& cursor = m_cursor_entity.EmplaceComponent<Components::Sprite>();
    cursor.color = glm::vec4(1.0F,1.0F, 1.0F, 1.0F);
    cursor.topLeftUV = {0.0F, 0.0F};
    cursor.bottomRightUV = {1.0F,1.0F};

    // loading a texture is still lot of work, but getting there.
    SDL::Image image(assetLoader.GetImageDir() + "/" + image_filename);
    SDL_GPUTextureCreateInfo textureInfo = {};
    textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.width = image.GetWidth();
    textureInfo.height = image.GetHeight();
    textureInfo.layer_count_or_depth = 1;
    textureInfo.num_levels = 1;
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    cursor.texture = std::make_shared<SDL::GpuTexture>(renderer.CreateTexture(textureInfo));
    cursor.texture->SetName("Cursor");

    SDL_GPUSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    samplerCreateInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerCreateInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.enable_anisotropy = true;
    samplerCreateInfo.max_anisotropy = 16; // NOLINT

    cursor.sampler = std::make_shared<SDL::GpuSampler>(renderer.CreateSampler(samplerCreateInfo));
    cursor.layer = (visible)? Components::RenderLayer::LAYER_GUI : Components::RenderLayer::LAYER_NONE;

    // texture transfer
    Systems::RenderSystem::TransferRequest request = {};
    request.cycle = false;
    request.type = Systems::RenderSystem::RequestType::UPLOAD_TO_TEXTURE;
    SDL_GPUTextureRegion& region = request.data.texture;
    region.texture = cursor.texture->Get();
    region.w = image.GetWidth();
    region.h = image.GetHeight();
    region.d = 1;
    request.p_src = image.GetPixels();

    renderer.UploadDataToBuffer({request});

    m_cursor_pos_px = m_window_size_px / 2.0F; // center of screen
    m_cursor_size_px = glm::vec2(region.w, region.h);

    UpdateCursor();
}