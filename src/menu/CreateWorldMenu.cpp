/**
 * @file CreateWorldMenu.cpp
 * @brief Menu for creating a new world.
 */

#include "CreateWorldMenu.hpp"
#include "MenuUtilities.hpp"
#include "components/Canvas.hpp"
#include "components/Renderable.hpp"
#include "components/Sprite.hpp"
#include "components/Transform.hpp"
#include "core/Logger.hpp"
#include "ecs/ECS.hpp"
#include "graphics/Texture2D.hpp"
#include "math/Hash.hpp"
#include "math/Voronoi.hpp"
#include "sdl/SDL.hpp"
#include "systems/RenderSystem.hpp"
#include "ui/Button.hpp"
#include "ui/Element.hpp"
#include "ui/HorizontalLayout.hpp"
#include "ui/Spacer.hpp"
#include "ui/TextInputBox.hpp"
#include "ui/VerticalLayout.hpp"
#include <SDL3/SDL_gpu.h>
#include <cstddef>
#include <glm/ext/vector_int2.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Menu {

CreateWorldMenu::CreateWorldMenu(Core::Engine& engine, MenuManager& manager, std::shared_ptr<UI::Style> p_style)
    : m_p_engine(&engine)
    , m_p_manager(&manager)
    , m_p_style(std::move(p_style)) {
}

void CreateWorldMenu::Activate() {

    ECS::Registry& registry = m_p_engine->GetEcsRegistry();
    m_entity = ECS::Entity(registry);

    Components::Canvas& canvas = m_entity.EmplaceComponent<Components::Canvas>();
    canvas.SetRenderMode(Components::Canvas::RenderMode::SCREEN);

    UI::VerticalLayout& root = canvas.EmplaceChild<UI::VerticalLayout>();
    root.EmplaceChild<UI::Spacer>();

    // Parameters pannel
    UI::HorizontalLayout& customizationPanel = root.EmplaceChild<UI::HorizontalLayout>();
    BuildCustomizationPanel(customizationPanel);
    root.EmplaceChild<UI::Spacer>();

    UI::HorizontalLayout& navigationPanel = root.EmplaceChild<UI::HorizontalLayout>();
    BuildNavigationPanel(navigationPanel);
}

void CreateWorldMenu::Deactivate() {
    m_entity = ECS::Entity();
    m_sprite = ECS::Entity();
}


void CreateWorldMenu::BuildCustomizationPanel(UI::Element& panelRoot) {

    UI::Element& background = AddBackground(m_p_style, panelRoot);
    background.SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN);

    UI::VerticalLayout& widgetList = background.EmplaceChild<UI::VerticalLayout>();

    // Main world generation
    UI::TextInputBox& seedInput = AddTextInputBox(m_p_style, widgetList, "World Seed", "Enter Seed", 32,
        [this](const std::string& textStr){
        this->m_world_parameters.m_seed = textStr;
    });
    seedInput.OnTextInput("Random");

    // Configure World Size (Tiles)
    AddSliderSelection(
        m_p_style,
        widgetList,
        "World Size",
        {"Small (64x64)", "Medium (128x128)", "Large (256x256)", "Huge (512x512)"},
        2, [this](size_t selection){
        this->m_world_parameters.m_dimmension = 64 << selection;
    });

    // Configure number of continents. Determines the number of continental plates to generate.
    AddSliderSelection(
        m_p_style,
        widgetList,
        "Number of Continents",
        {"2", "4", "8", "16"}, 2, [this](size_t selection){
        this->m_world_parameters.m_num_continents = 2 << selection;
    });

    // Configure percent of world that is land. Determines the number of oceanic plates to generate
    AddSliderSelection(
        m_p_style,
        widgetList,
        "Percent Land",
        {"30%", "40%", "50%", "60%", "70%"},
        0U,
        [this](size_t selection){
        this->m_world_parameters.m_percent_land = 30.0F + 10.0F * static_cast<float>(selection);
    });

    // Configure average size of each region, this is used to calculate the number of regions to create on map
    // given the world size.
    AddSliderSelection(
        m_p_style,
        widgetList,
        "Average Region Size",
        {"4", "8", "16", "32", "64", "128"},
        3,
        [this](size_t selection) {
            this->m_world_parameters.m_region_size = 4 << selection;
        }
    );
    // Configure Temperature
    // Configure Rainfall

    // Civilization generation

    // ensure menu is left-aligned
    panelRoot.EmplaceChild<UI::Spacer>();

}

void CreateWorldMenu::BuildNavigationPanel(UI::Element& panelRoot) {

    panelRoot.EmplaceChild<UI::Spacer>();

    AddButton(m_p_style, panelRoot, "Back", UI::ButtonState::ENABLED,
        [this](){ m_p_manager->RequestChangeActiveMenu("ChooseWorld"); });

    AddButton(
        m_p_style,
        panelRoot,
        "Generate",
        UI::ButtonState::ENABLED,
        [this](){this->GenerateWorld();});

    panelRoot.EmplaceChild<UI::Spacer>();
}

void CreateWorldMenu::GenerateWorld() {

    size_t numTiles = m_world_parameters.m_dimmension * m_world_parameters.m_dimmension;
    int regionCount = static_cast<int>(numTiles / m_world_parameters.m_region_size);
    glm::ivec2 canvasSize = glm::ivec2(static_cast<int>(m_world_parameters.m_dimmension)) * 10; // NOLINT
    Math::VoronoiGraph graph = Math::VoronoiGenerator::Generate(
        regionCount,
        canvasSize,
        static_cast<int>(m_world_parameters.m_dimmension),
        Math::HashFNV1A(m_world_parameters.m_seed));

    uint32_t width = static_cast<uint32_t>(m_world_parameters.m_dimmension);
    uint32_t height = width;
    // now generate image
    std::vector<uint8_t> pixels = graph.ToPixels({width, height});

    Systems::RenderSystem& renderSystem = m_p_engine->GetEcsRegistry().GetSystem<Systems::RenderSystem>();
    SDL_GPUSamplerCreateInfo samplerInfo = {};
    samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerInfo.enable_anisotropy = true;
    samplerInfo.max_anisotropy = 16;
    samplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.min_filter = SDL_GPU_FILTER_LINEAR;

    // Create sampler
    std::shared_ptr<SDL::GpuSampler> p_sampler = std::make_shared<SDL::GpuSampler>(
        renderSystem.CreateSampler(samplerInfo));

    std::shared_ptr<Graphics::Texture2D> texture = std::make_shared<Graphics::Texture2D>(
        *m_p_engine,
        std::move(p_sampler),
        width,
        height,
        true);

    texture->LoadImageData(pixels, width, height);

    // Create the sprite entity.
    m_sprite = ECS::Entity(m_p_engine->GetEcsRegistry());

    Components::Sprite& sprite = m_sprite.EmplaceComponent<Components::Sprite>();
    sprite.texture = texture;
    sprite.layer = Components::RenderLayer::LAYER_3D_OPAQUE;


    m_sprite.EmplaceComponent<Components::Transform>()
        .Translate({0.0F, 0.0F, -1.0F});
}
}