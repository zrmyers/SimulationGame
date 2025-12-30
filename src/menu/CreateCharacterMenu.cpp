#include "CreateCharacterMenu.hpp"
#include "MenuUtilities.hpp"
#include "components/Canvas.hpp"
#include "components/Creature.hpp"
#include "components/Transform.hpp"
#include "core/Logger.hpp"
#include "creature/Compendium.hpp"
#include "ecs/ECS.hpp"
#include "systems/CreatureSystem.hpp"
#include "systems/InventorySystem.hpp"
#include "ui/Button.hpp"
#include "ui/ButtonStyle.hpp"
#include "ui/CheckBox.hpp"
#include "ui/CheckBoxStyle.hpp"
#include "ui/Element.hpp"
#include "ui/HorizontalLayout.hpp"
#include "ui/Spacer.hpp"
#include "ui/VerticalLayout.hpp"
#include <memory>
#include <string>

namespace Menu {

CreateCharacterMenu::CreateCharacterMenu(Core::Engine& engine, MenuManager& manager, std::shared_ptr<UI::Style> p_style)
    : m_p_engine(&engine)
    , m_p_manager(&manager)
    , m_p_style(std::move(p_style)) {
}

void CreateCharacterMenu::Activate() {

    Core::Engine* p_engine = m_p_engine;
    MenuManager* p_menuManager = m_p_manager;
    Systems::CreatureSystem& creatureSystem = m_p_engine->GetEcsRegistry().GetSystem<Systems::CreatureSystem>();
    Systems::InventorySystem& inventorySystem = m_p_engine->GetEcsRegistry().GetSystem<Systems::InventorySystem>();
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

    // skin tone selection

    // hair selection

    // beard selection

    // hair tone selection
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

void CreateCharacterMenu::SetCharacterSex(bool is_male) {

    Systems::CreatureSystem& creatureSystem = m_p_engine->GetEcsRegistry().GetSystem<Systems::CreatureSystem>();
    Components::CreatureInstance& creature = m_entity.GetComponent<Components::CreatureInstance>();

    // replace the creature with a new one
    creature = creatureSystem.MakeCreature(creature.m_p_species->m_name, is_male);
}

bool CreateCharacterMenu::GetCharacterSex() {

    Components::CreatureInstance& creature = m_entity.GetComponent<Components::CreatureInstance>();
    return creature.GetSex();
}

}