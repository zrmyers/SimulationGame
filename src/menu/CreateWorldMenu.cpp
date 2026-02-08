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
#include "ecs/ECS.hpp"
#include "graphics/Texture2D.hpp"
#include "menu/MenuUtilities.hpp"
#include "sdl/SDL.hpp"
#include "systems/RenderSystem.hpp"
#include "ui/Button.hpp"
#include "ui/ButtonStyle.hpp"
#include "ui/Element.hpp"
#include "ui/HorizontalLayout.hpp"
#include "ui/Spacer.hpp"
#include "ui/TextInputBox.hpp"
#include "ui/VerticalLayout.hpp"
#include "world/WorldGenerator.hpp"
#include <SDL3/SDL_gpu.h>
#include <cstddef>
#include <glm/ext/vector_int2.hpp>
#include <memory>
#include <string>
#include <vector>
#include "world/MapOverlay.hpp"
#include "world/WorldSave.hpp"
#include "core/SeedWords.hpp"

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

    UI::HorizontalLayout& overlaySelection = root.EmplaceChild<UI::HorizontalLayout>();
    overlaySelection.EmplaceChild<UI::Spacer>();
    AddSliderSelection(
        m_p_style,
        overlaySelection,
        "Overlay",
        {"Tectonic Plates", "Height Map", "Water Map", "Heat Map", "Moisture Map", "Biome Map"},
        static_cast<size_t>(m_selected_overlay),
        [this](size_t selection){
            this->SetOverlay(static_cast<World::OverlayType>(selection));
    });
    overlaySelection.EmplaceChild<UI::Spacer>();

    UI::HorizontalLayout& navigationPanel = root.EmplaceChild<UI::HorizontalLayout>();
    BuildNavigationPanel(navigationPanel);
}

void CreateWorldMenu::Deactivate() {
    m_entity = ECS::Entity();
    m_sprite = ECS::Entity();
    m_p_world = nullptr;
    m_selected_overlay = World::OverlayType::PLATE_TECTONICS;
    m_p_done_button = nullptr;
}


void CreateWorldMenu::BuildCustomizationPanel(UI::Element& panelRoot) {

    UI::Element& background = AddBackground(m_p_style, panelRoot);
    background.SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN);

    UI::VerticalLayout& widgetList = background.EmplaceChild<UI::VerticalLayout>();

    UI::TextInputBox& worldName = AddTextInputBox(m_p_style, widgetList, "World Name", "Enter Name", 32,
        [this](const std::string& textStr) {
            this->m_world_parameters.SetName(textStr);
        }
    );
    worldName.InsertText(m_p_engine->GetNameGenerator("Regions").Generate(32U));

    // Main world generation
    UI::TextInputBox& seedInput = AddTextInputBox(m_p_style, widgetList, "World Seed", "Enter Seed", 32,
        [this](const std::string& textStr){
        this->m_world_parameters.SetSeedAscii(textStr);
    });
    seedInput.InsertText(Core::SeedWords::ChooseRandomSeedWord());

    // Configure World Size (Tiles)
    AddSliderSelection(
        m_p_style,
        widgetList,
        "World Size",
        {"Small (64x64)", "Medium (128x128)", "Large (256x256)", "Huge (512x512)"},
        3, [this](size_t selection){
        this->m_world_parameters.SetDimension(64 << selection);
    });

    // Configure number of continents. Determines the number of continental plates to generate.
    AddSliderSelection(
        m_p_style,
        widgetList,
        "Number of Continents",
        {"2", "4", "8", "16"}, 3, [this](size_t selection){
        this->m_world_parameters.SetNumContinents(2 << selection);
    });

    // Configure percent of world that is land. Determines the number of oceanic plates to generate
    AddSliderSelection(
        m_p_style,
        widgetList,
        "Percent Land",
        {"30%", "40%", "50%", "60%", "70%"},
        1U,
        [this](size_t selection){
        this->m_world_parameters.SetPercentLand(30.0F + (10.0F * static_cast<float>(selection)));
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
            this->m_world_parameters.SetRegionSize(4 << selection);
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
        [this](){ m_p_manager->ReturnToPreviousMenu(); });

    AddButton(
        m_p_style,
        panelRoot,
        "Generate",
        UI::ButtonState::ENABLED,
        [this](){this->GenerateWorld();});

    m_p_done_button = &AddButton(
        m_p_style,
        panelRoot,
        "Done",
        UI::ButtonState::DISABLED,
        [this](){
            this->SaveWorld();
            m_p_manager->ReturnToPreviousMenu();
    });

    panelRoot.EmplaceChild<UI::Spacer>();
}

void CreateWorldMenu::GenerateWorld() {

    m_p_world = std::move(World::WorldGenerator::Generate(m_world_parameters));

    SetOverlay(m_selected_overlay);

    m_p_done_button->SetButtonState(UI::ButtonState::ENABLED);
}

void CreateWorldMenu::SetOverlay(World::OverlayType selection) {

    if (m_p_world) {
        uint32_t width = m_world_parameters.GetDimension();
        uint32_t height = width;
        // now generate image
        std::vector<uint8_t> pixels = World::MapOverlay::GetOverlay(*m_p_world, selection);

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

    m_selected_overlay = selection;
}

}


void Menu::CreateWorldMenu::SaveWorld() {

    World::SaveWorldToFile( *m_p_world);
}