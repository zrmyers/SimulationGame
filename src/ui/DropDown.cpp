#include "DropDown.hpp"
#include "ButtonStyle.hpp"
#include "Element.hpp"
#include "components/Canvas.hpp"
#include "core/Logger.hpp"
#include "ecs/ECS.hpp"
#include "ui/Button.hpp"
#include "ui/DropDown.hpp"
#include "ui/VerticalLayout.hpp"
#include <memory>

UI::DropDown::DropDown()
    : m_selected(0)
    , m_p_selection_button(nullptr)
    , m_selection_in_progress(false) {
    SetLayoutMode(LayoutMode::FIT_TO_CHILDREN);
}

UI::DropDown& UI::DropDown::SetStyle(const std::shared_ptr<DropDownStyle>& p_style) {
    m_p_style = p_style;
    return *this;
}

UI::DropDown& UI::DropDown::SelectOption(size_t select) {

    if (select < m_options.size() && m_p_style != nullptr) {

        if (m_p_selection_button == nullptr) {

            UI::Button& selection = EmplaceChild<UI::Button>();
            selection.SetButtonStyle(m_p_style->GetSelectionButtonStyle())
                .SetText(m_options.at(select))
                .SetOnClickCallback([this](){
                    m_selection_in_progress = true;
                })
                .SetButtonState(ButtonState::ENABLED);
            m_p_selection_button = &selection;
        }
        else {
            m_p_selection_button->SetText(m_options.at(select));
        }

        m_selected = select;
        m_selection_in_progress = false;
        SetDirty();

        if (m_value_change_callback) {
            m_value_change_callback(m_selected);
        }
    }
    return *this;
}

UI::DropDown& UI::DropDown::SetOptions(std::vector<std::string> options) {
    m_options = std::move(options);
    return *this;
}

UI::DropDown& UI::DropDown::SetValueChangedCallback(DropDownValueChangeCallback_t callback) {
    m_value_change_callback = std::move(callback);
    return *this;
}

void UI::DropDown::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) {

    if (m_selection_in_progress && !m_dropdown_entity.IsValid()) {

        // The dropdown menu will be processed next frame.
        SpawnDropDown(registry, screenSize, depth);
    }
    else if (!m_selection_in_progress && m_dropdown_entity.IsValid()) {

        DespawnDropDown(registry, screenSize, depth);
    }

    depth++;
    for (auto& p_child : GetChildren()) {
        p_child->UpdateGraphics(registry, screenSize, depth);
    }
}

void UI::DropDown::SpawnDropDown(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) {

    // How wide to make the button
    float optionsWidth = GetAbsoluteSize().x;

    m_dropdown_entity = ECS::Entity(registry);
    Components::Canvas& canvas = m_dropdown_entity.FindOrEmplaceComponent<Components::Canvas>();
    canvas.SetRenderMode(Components::Canvas::RenderMode::SCREEN);

    uint8_t canvasDepth = GetCanvasDepth(depth);
    canvasDepth++;
    canvas.SetDepth(canvasDepth);

    UI::VerticalLayout& optionsList = canvas.EmplaceChild<UI::VerticalLayout>();
    optionsList.SetOffsetPosition(GetAbsolutePosition()+ glm::vec2(0.0F, 96.0F));

    const std::shared_ptr<ButtonStyle>& p_options_style = m_p_style->GetOptionsButtonStyle();

    size_t optionIndex = 0U;
    for (const std::string& option : m_options) {

        UI::Button& optionButton = optionsList.EmplaceChild<UI::Button>();
        optionButton.SetButtonStyle(p_options_style)
            .SetText(option)
            .SetButtonState((optionIndex == m_selected)? UI::ButtonState::SELECTED : UI::ButtonState::ENABLED)
            .SetOnClickCallback([this, optionIndex](){
                this->SelectOption(optionIndex);
            })
            .SetFixedSize({optionsWidth, 96.0F})
            .SetLayoutMode(UI::LayoutMode::FIXED);
        optionIndex++;
    }

    optionsList.CalculateSize(screenSize);
    optionsList.CalculatePosition(screenSize, glm::vec2(0.0F));

}

void UI::DropDown::DespawnDropDown(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) {
    m_dropdown_entity = ECS::Entity();
}