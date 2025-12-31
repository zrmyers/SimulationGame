#include "CreateCharacterMenu.hpp"
#include "MenuUtilities.hpp"
#include "components/Canvas.hpp"
#include "components/Creature.hpp"
#include "components/Transform.hpp"
#include "components/InputHandler.hpp"
#include "core/Logger.hpp"
#include "creature/Compendium.hpp"
#include "ecs/ECS.hpp"
#include "systems/CreatureSystem.hpp"
#include "systems/InventorySystem.hpp"
#include "systems/RenderSystem.hpp"
#include "ui/Button.hpp"
#include "ui/ButtonStyle.hpp"
#include "ui/CheckBox.hpp"
#include "ui/CheckBoxStyle.hpp"
#include "ui/Element.hpp"
#include "ui/HorizontalLayout.hpp"
#include "ui/Spacer.hpp"
#include "ui/VerticalLayout.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_mouse.h>
#include <glm/ext/scalar_constants.hpp>
#include <memory>
#include <string>

namespace Menu {

CreateCharacterMenu::CreateCharacterMenu(Core::Engine& engine, MenuManager& manager, std::shared_ptr<UI::Style> p_style)
    : m_p_engine(&engine)
    , m_p_manager(&manager)
    , m_p_style(std::move(p_style))
    , m_track_mouse(false)
    , m_prev_position(0.0F)
    , m_original_transform(1.0F) {
}

void CreateCharacterMenu::Activate() {

    Core::Engine* p_engine = m_p_engine;
    MenuManager* p_menuManager = m_p_manager;
    Systems::CreatureSystem& creatureSystem = m_p_engine->GetEcsRegistry().GetSystem<Systems::CreatureSystem>();
    Systems::InventorySystem& inventorySystem = m_p_engine->GetEcsRegistry().GetSystem<Systems::InventorySystem>();
    Systems::RenderSystem& renderSystem = m_p_engine->GetEcsRegistry().GetSystem<Systems::RenderSystem>();
    ECS::Registry& registry = m_p_engine->GetEcsRegistry();

    m_entity = ECS::Entity(registry);

    // Instantiate the character's creature model.
    Components::CreatureInstance& creatureInstance = m_entity.EmplaceComponent<Components::CreatureInstance>();
    creatureInstance = creatureSystem.MakeCreature("human");

    // Set the character's location.
    Components::Transform& transform = m_entity.EmplaceComponent<Components::Transform>();
    transform.Translate(glm::vec3(0.0F, -1.0F, -1.0F));

    // start building the menu.
    Components::Canvas& canvas = m_entity.EmplaceComponent<Components::Canvas>();
    canvas.SetRenderMode(Components::Canvas::RenderMode::SCREEN);

    canvas.SetMouseButtonPressCallback([this](UI::MouseButtonID button){
        if (button == UI::MouseButtonID::MOUSE_LEFT) {
            m_track_mouse = true;

            // initialize mouse state for tracking.
            (void) SDL_GetMouseState(&m_prev_position.x, &m_prev_position.y);

            Components::Transform& transform = m_entity.GetComponent<Components::Transform>();
            m_original_transform = transform.m_transform;
        }
    });
    canvas.SetMouseButtonReleaseCallback([this](UI::MouseButtonID button){
        if (button == UI::MouseButtonID::MOUSE_RIGHT) {
            m_track_mouse = false;
        }
    });
    canvas.SetHoverEnterCallback([this](){
        if (m_track_mouse) {
            ProcessMouseMovement();
        }
    });


    // populate canvas with ui elements
    UI::VerticalLayout& mainVertical = canvas.EmplaceChild<UI::VerticalLayout>();
    mainVertical.EmplaceChild<UI::Spacer>();

    // Top widget
    UI::HorizontalLayout& horizontal1 = mainVertical.EmplaceChild<UI::HorizontalLayout>();

    BuildCustomizationPanel(horizontal1);
    horizontal1.EmplaceChild<UI::Spacer>();

    mainVertical.EmplaceChild<UI::Spacer>();

    // Bottom widget
    UI::HorizontalLayout& horizontal2 = mainVertical.EmplaceChild<UI::HorizontalLayout>();

    horizontal2.EmplaceChild<UI::Spacer>();

    BuildFinalizationPanel(horizontal2);
    horizontal2.EmplaceChild<UI::Spacer>();
}

void CreateCharacterMenu::Deactivate() {

    m_entity = ECS::Entity();
}


void CreateCharacterMenu::BuildCustomizationPanel(UI::Element& panelRoot) {

    // Get reference to creature
    Components::CreatureInstance& creatureInstance = m_entity.GetComponent<Components::CreatureInstance>();

    UI::Element& background = Menu::AddBackground(m_p_style, panelRoot);
    background.SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN);

    UI::VerticalLayout& widgetList = background.EmplaceChild<UI::VerticalLayout>();

    // variant selection
    AddRadioSelection(m_p_style, widgetList, "Sex",{"Male", "Female"} , creatureInstance.m_variant_id,
        [this](size_t selected_index){
        this->SetCharacterSex(selected_index == 0);
    });

    // material sliders
    const Creature::Material& skinMaterial = creatureInstance.m_p_species->GetMaterialByName("skin");
    AddMaterialCustomizer(widgetList, skinMaterial, "Skin Tone", [this](size_t selection){
        this->SetSkinColor(selection);
    });

    // eye color selection
    const Creature::Material& eyeMaterial = creatureInstance.m_p_species->GetMaterialByName("eyes");
    AddMaterialCustomizer(widgetList, eyeMaterial, "Eye Color", [this](size_t selection){
        this->SetEyeColor(selection);
    });

    // hair tone selection
    const Creature::Material& hairMaterial = creatureInstance.m_p_species->GetMaterialByName("hair");
    AddMaterialCustomizer(widgetList, hairMaterial, "Hair Color", [this](size_t selection){
        this->SetHairColor(selection);
    });

    // hair selection

    // beard selection

}

void CreateCharacterMenu::BuildFinalizationPanel(UI::Element& panelRoot) {

    MenuManager* p_menuManager = m_p_manager;
    UI::Element& background = Menu::AddBackground(m_p_style, panelRoot);
    UI::HorizontalLayout& characterManagement = background.EmplaceChild<UI::HorizontalLayout>();

    AddButton(m_p_style, characterManagement, "Cancel", UI::ButtonState::ENABLED,
        [p_menuManager](){p_menuManager->RequestChangeActiveMenu("ChooseCharacter");});
    AddButton(m_p_style, characterManagement, "Done", UI::ButtonState::DISABLED,
        [p_menuManager](){
            // save character to file
            // return to previous screen
            p_menuManager->RequestChangeActiveMenu("ChooseCharacter");
        });
}

void CreateCharacterMenu::AddMaterialCustomizer(UI::Element& root, const Creature::Material& material, const std::string& fieldName, SelectionChangeCallback_t callback) {

    std::vector<std::string> palleteOptions;
    palleteOptions.reserve(material.m_pallete.size());
    for (const Creature::ColorPallete& pallete : material.m_pallete) {
        palleteOptions.push_back(pallete.m_name);
    }

    AddSliderSelection(m_p_style, root, fieldName,
        palleteOptions, 0, std::move(callback));
}

void CreateCharacterMenu::SetCharacterSex(bool is_male) {

    Systems::CreatureSystem& creatureSystem = m_p_engine->GetEcsRegistry().GetSystem<Systems::CreatureSystem>();
    Components::CreatureInstance& creature = m_entity.GetComponent<Components::CreatureInstance>();
    Components::CreatureInstance oldCreature = std::move(creature);

    // replace the creature with a new one
    creature = creatureSystem.MakeCreature(creature.m_p_species->m_name, is_male);

    // transfer materials from previous creature.
    creature.m_material_instance = std::move(oldCreature.m_material_instance);
}

bool CreateCharacterMenu::GetCharacterSex() {

    Components::CreatureInstance& creature = m_entity.GetComponent<Components::CreatureInstance>();
    return creature.GetSex();
}

void CreateCharacterMenu::SetSkinColor(size_t selected_pallete) {

    Components::CreatureInstance& creature = m_entity.GetComponent<Components::CreatureInstance>();

    for (Creature::MaterialInstance& materialInstance : creature.m_material_instance) {

        const Creature::Material& material = creature.m_p_species->m_materials.at(materialInstance.m_index);
        if (material.m_name == "skin") {

            const Creature::ColorPallete& colorPallete = material.m_pallete.at(selected_pallete);
            colorPallete.Apply(materialInstance.m_data);
            materialInstance.m_pallete_index = selected_pallete;
        }
    }
}

void CreateCharacterMenu::SetEyeColor(size_t selected_pallete) {

    Components::CreatureInstance& creature = m_entity.GetComponent<Components::CreatureInstance>();

    for (Creature::MaterialInstance& materialInstance : creature.m_material_instance) {

        const Creature::Material& material = creature.m_p_species->m_materials.at(materialInstance.m_index);
        if (material.m_name == "eyes") {

            const Creature::ColorPallete& colorPallete = material.m_pallete.at(selected_pallete);
            colorPallete.Apply(materialInstance.m_data);
            materialInstance.m_pallete_index = selected_pallete;
        }
    }
}

void CreateCharacterMenu::SetHairColor(size_t selected_pallete) {

    Components::CreatureInstance& creature = m_entity.GetComponent<Components::CreatureInstance>();

    for (Creature::MaterialInstance& materialInstance : creature.m_material_instance) {

        const Creature::Material& material = creature.m_p_species->m_materials.at(materialInstance.m_index);
        if (material.m_name == "hair") {

            const Creature::ColorPallete& colorPallete = material.m_pallete.at(selected_pallete);
            colorPallete.Apply(materialInstance.m_data);
            materialInstance.m_pallete_index = selected_pallete;
        }
    }
}


void CreateCharacterMenu::ProcessMouseMovement() {

    float posX = 0.0F;
    float posY = 0.0F;

    // check if mouse is still being pressed
    SDL_MouseButtonFlags flags = SDL_GetMouseState(&posX, &posY);
    if ((flags & SDL_BUTTON_LMASK) == 0) {

        // Done tracking mouse
        m_track_mouse = false;
    } else {
        Systems::RenderSystem& renderSystem = m_p_engine->GetEcsRegistry().GetSystem<Systems::RenderSystem>();
        Components::Transform& transform = m_entity.GetComponent<Components::Transform>();

        glm::vec2 resolution = renderSystem.GetWindowSize();
        resolution /= 2.0F; // half-dims

        // determine motion direction
        float xMovement = posX - m_prev_position.x;
        float yMovement = posY - m_prev_position.y;

        // reset transform to beginning of tracking.
        transform.m_transform = m_original_transform;

        // calculate rotation angle, normalized to half a full rotation in either direction if starting from center
        float rotationAngle = glm::pi<float>() * (xMovement / resolution.x);

        // Rotate Left
        transform.Rotate(rotationAngle, glm::vec3(0.0F, 1.0F, 0.0F));

        // calculate translation on +/- y axis
        float translationDistance = yMovement / resolution.y;
        transform.Translate(glm::vec3(0.0F, -translationDistance, 0.0F));

    }

}

}