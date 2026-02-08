/**
 * @file ChooseWorldMenu.cpp
 * @brief Menu for selecting which world to play in.
 */

#include "ChooseWorldMenu.hpp"
#include "MenuUtilities.hpp"
#include "components/Canvas.hpp"
#include "components/Renderable.hpp"
#include "components/Sprite.hpp"
#include "components/Transform.hpp"
#include "ui/Button.hpp"
#include "ui/ButtonStyle.hpp"
#include "ui/HorizontalLayout.hpp"
#include "ui/Spacer.hpp"
#include "ui/TextElement.hpp"
#include "ui/VerticalLayout.hpp"
#include "core/Logger.hpp"
#include "world/WorldSave.hpp"
#include "world/MapOverlay.hpp"
#include "graphics/Texture2D.hpp"
#include "sdl/SDL.hpp"
#include "systems/RenderSystem.hpp"
#include <glm/ext/vector_int2.hpp>

namespace Menu {
ChooseWorldMenu::ChooseWorldMenu(Core::Engine& engine, MenuManager& manager, std::shared_ptr<UI::Style> p_style)
    : m_p_engine(&engine)
    , m_p_manager(&manager)
    , m_p_style(std::move(p_style)) {
}

void ChooseWorldMenu::Activate() {

    ECS::Registry& registry = m_p_engine->GetEcsRegistry();
    m_entity = ECS::Entity(registry);

    m_selected_world_index = 0U;
    m_worlds = World::GetSavedWorlds();

    Components::Canvas& canvas = m_entity.EmplaceComponent<Components::Canvas>();
    canvas.SetRenderMode(Components::Canvas::RenderMode::SCREEN);

    UI::VerticalLayout& vertical = canvas.EmplaceChild<UI::VerticalLayout>();

    vertical.EmplaceChild<UI::Spacer>();

    // This is bar for choosing between various saved characters, if they exist
    UI::HorizontalLayout& worldRotary = vertical.EmplaceChild<UI::HorizontalLayout>();
    worldRotary.EmplaceChild<UI::Spacer>();
    AddButton(
        m_p_style,
        worldRotary,
        "<",
        m_worlds.empty()? UI::ButtonState::DISABLED : UI::ButtonState::ENABLED,
        [this](){ this->SelectWorld(false); });
    UI::TextElement& worldName = AddTextElement(
        m_p_style,
        worldRotary,
        "None",
        glm::vec4(1.0F, 1.0F, 1.0F, 1.0F),
        20U);
    m_p_world_name_element = &worldName;
    AddButton(
        m_p_style,
        worldRotary,
        ">",
        m_worlds.empty()? UI::ButtonState::DISABLED : UI::ButtonState::ENABLED,
        [this](){ this->SelectWorld(true); });
    worldRotary.EmplaceChild<UI::Spacer>();

    UI::HorizontalLayout& bottomBar = vertical.EmplaceChild<UI::HorizontalLayout>();
    bottomBar.EmplaceChild<UI::Spacer>();

    AddButton(m_p_style, bottomBar, "Back", UI::ButtonState::ENABLED,
        [this](){ m_p_manager->ReturnToPreviousMenu(); });

    AddButton(m_p_style, bottomBar, "Start", UI::ButtonState::DISABLED,
        [this](){
            Core::Logger::Info("Play requested for world: " + m_worlds.at(m_selected_world_index));;
        });
    AddButton(m_p_style, bottomBar, "Remove", m_worlds.empty()? UI::ButtonState::DISABLED : UI::ButtonState::ENABLED,
        [this](){
            this->DeleteSelectedWorld();
        });
    AddButton(m_p_style, bottomBar, "New", UI::ButtonState::ENABLED,
        [this](){
            m_p_manager->RequestChangeActiveMenu("CreateWorld");
        });

    bottomBar.EmplaceChild<UI::Spacer>();

    SelectWorld(m_selected_world_index);
}

void ChooseWorldMenu::Deactivate() {
    m_entity = ECS::Entity();
    m_sprite = ECS::Entity();
    m_worlds.clear();
    m_selected_world_index = 0U;
    m_p_world_name_element = nullptr;
    m_p_world = nullptr;
}


void ChooseWorldMenu::SelectWorld(size_t worldIndex) {
    if (worldIndex >= m_worlds.size()) {
        return;
    }
    m_selected_world_index = worldIndex;

    const std::string& worldName = m_worlds.at(m_selected_world_index);
    m_p_world_name_element->SetTextString(worldName);

    // Load preview image of world.
    m_p_world = World::LoadWorldFromFile(worldName);

    if (m_p_world) {
        uint32_t width = m_p_world->GetParameters().GetDimension();
        uint32_t height = width;

        // Get the biome overlay pixels
        std::vector<uint8_t> pixels = World::MapOverlay::GetOverlay(*m_p_world, World::OverlayType::BIOME_MAP);

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

        // Create the sprite entity for the preview
        m_sprite = ECS::Entity(m_p_engine->GetEcsRegistry());

        Components::Sprite& sprite = m_sprite.EmplaceComponent<Components::Sprite>();
        sprite.texture = texture;
        sprite.layer = Components::RenderLayer::LAYER_3D_OPAQUE;

        m_sprite.EmplaceComponent<Components::Transform>()
            .Translate({0.0F, 0.0F, -1.0F});
    }
}

void ChooseWorldMenu::SelectWorld(bool next) {

    // Determine which character to select
    if (next) {

        if (m_selected_world_index + 1 >= m_worlds.size()) {
            m_selected_world_index = 0U;
        }
        else {
            m_selected_world_index++;
        }
    }
    else {
        if (m_selected_world_index == 0U) {
            m_selected_world_index = m_worlds.size() - 1;
        }
        else {
            m_selected_world_index--;
        }
    }
    SelectWorld(m_selected_world_index);
}


void ChooseWorldMenu::DeleteSelectedWorld() {

    // Delete the file
    World::DeleteWorld(m_worlds.at(m_selected_world_index));

    // update the file list.
    std::vector<std::string> updatedCharacterFiles;
    updatedCharacterFiles.reserve(m_worlds.size());
    for (size_t index = 0U; index < m_worlds.size(); index++) {

        if (index != m_selected_world_index) {

            updatedCharacterFiles.push_back(std::move(m_worlds.at(index)));
        }
    }
    m_worlds = std::move(updatedCharacterFiles);

    if (m_worlds.empty()) {

        // Need to reset all buttons to disabled state. Maybe we can cheat and deactivate then reactivate menu
        Deactivate();
        Activate();
    }
    else {
        // choose the next character
        if (m_selected_world_index >= m_worlds.size()) {

            m_selected_world_index = m_worlds.size() - 1U;
        }

        // Select the character
        SelectWorld(m_selected_world_index);
    }
}

}