#include "Radio.hpp"
#include "CheckBoxStyle.hpp"
#include "Element.hpp"
#include "HorizontalLayout.hpp"
#include "TextElement.hpp"
#include "graphics/Font.hpp"
#include "ui/CheckBox.hpp"

namespace UI {

Radio::Radio()
    : m_selected(0U)
    , m_p_selected(nullptr) {
}

Radio& Radio::SetStyle(const std::shared_ptr<RadioStyle>& p_style) {
    m_p_style = p_style;
    return *this;
}

Radio& Radio::SelectOption(size_t select) {

    if (select < m_options.size() && m_p_style != nullptr) {

        if (m_p_buttons.empty()) {

            UI::HorizontalLayout& horizontal = EmplaceChild<UI::HorizontalLayout>();
            horizontal.SetLayoutMode(LayoutMode::FIT_TO_CHILDREN);
            // initialize all of the buttons
            for (size_t index = 0; index < m_options.size(); index++) {

                UI::CheckBox& checkbox = horizontal.EmplaceChild<UI::CheckBox>();
                checkbox.SetStyle(m_p_style->GetCheckboxStyle())
                    .SetCheckBoxStateCallback([index, this](bool state){
                        this->SelectOption(index);
                    })
                    .SetCheckBoxState((index == select)? CheckBoxState::ON : CheckBoxState::OFF)
                    .SetToggleOffDisabled(true)
                    .SetLayoutMode(LayoutMode::FIXED)
                    .SetFixedSize(glm::vec2(32.0F, 32.0F)); // todo this should come from somewhere else.

                m_p_buttons.push_back(&checkbox);

                UI::TextElement& text = horizontal.EmplaceChild<UI::TextElement>();
                const std::shared_ptr<Graphics::Font>& p_font = m_p_style->GetFont();
                text.SetFont(p_font);
                text.SetText(p_font->CreateText(m_options.at(index)));
                text.SetFixedSize(text.GetTextSize());
                text.SetOrigin({0.5F, 0.5F});
                text.SetLayoutMode(LayoutMode::FIXED);
                text.SetRelativePosition({0.5F, 0.5F});
            }
        } else {

            m_p_selected->SetCheckBoxState(CheckBoxState::OFF);
        }

        m_selected = select;
        m_p_selected = m_p_buttons.at(m_selected);

        SetDirty();

        if (m_value_change_callback) {
            m_value_change_callback(m_selected);
        }
    }
    return *this;
}

Radio& Radio::SetOptions(std::vector<std::string> options) {
    m_options = std::move(options);
    return *this;
}

Radio& Radio::SetValueChangedCallback(RadioValueChangeCallback_t callback) {
    m_value_change_callback = std::move(callback);
    return *this;
}

void Radio::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) {

    depth++;
    for (auto& p_child : GetChildren()) {
        p_child->UpdateGraphics(registry, screenSize, depth);
    }
}

}