#include "MenuUtilities.hpp"
#include "ui/CheckBox.hpp"
#include "ui/Element.hpp"
#include "ui/HorizontalLayout.hpp"
#include "ui/NineSlice.hpp"
#include "ui/Slider.hpp"
#include "ui/SliderStyle.hpp"
#include "ui/Spacer.hpp"
#include "ui/TextInputBox.hpp"
#include "ui/VerticalLayout.hpp"
#include "ui/Radio.hpp"

namespace Menu {

UI::Element& AddBackground(
    const std::shared_ptr<UI::Style>& p_style,
    UI::Element& parent) {

    UI::NineSlice& background = parent.EmplaceChild<UI::NineSlice>();
    background.SetStyle(p_style->GetNineSliceStyle("box-style"))
        .SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN);

    return background;
}

UI::Button& AddButton(
    const std::shared_ptr<UI::Style>& p_style,
    UI::Element& parent,
    const std::string& text_label,
    UI::ButtonState state,
    UI::OnClickCallback callback) {

    UI::Button& button = parent.EmplaceChild<UI::Button>();
    button.SetButtonStyle(p_style->GetButtonStyle("simple"))
        .SetText(text_label)
        .SetButtonState(state)
        .SetOnClickCallback(std::move(callback))
        .SetFixedSize({256.0F, 96.0F})
        .SetLayoutMode(UI::LayoutMode::FIXED);
    return button;
}


UI::TextElement& AddTextElement(
    const std::shared_ptr<UI::Style>& p_style,
    UI::Element& parent,
    const std::string& text,
    const glm::vec4& color,
    size_t max_characters) {

    const std::shared_ptr<Graphics::Font>& p_font = p_style->GetFont("Default-UI");
    glm::vec2 maxTextSize = p_font->GetMaxGlyphSizePx();
    maxTextSize.x *= static_cast<float>(max_characters);
    UI::Spacer& reservedSpace = parent.EmplaceChild<UI::Spacer>();
    reservedSpace.SetFixedSize(maxTextSize)
        .SetLayoutMode(UI::LayoutMode::FIXED)
        .SetRelativePosition({0.5F, 0.5F})
        .SetOrigin({0.5F, 0.5F});

    UI::TextElement& textElement = reservedSpace.EmplaceChild<UI::TextElement>();
    textElement.SetFont(p_font)
        .SetText(p_font->CreateText(text))
        .SetTextColor(color)
        .SetFixedSize(textElement.GetTextSize())
        .SetLayoutMode(UI::LayoutMode::FIXED)
        .SetOrigin({0.5F, 0.5F})
        .SetRelativePosition({0.5F, 0.5F});

    return textElement;
}

void AddRadioSelection(
    const std::shared_ptr<UI::Style>& p_style,
    UI::Element& parent,
    const std::string& fieldName, // name of field being modified
    const std::vector<std::string>& options, // possible options to select from
    size_t selected_index, // currently selected option
    SelectionChangeCallback_t callback // callback when selection changes.
) {
    UI::VerticalLayout& selectionField = parent.EmplaceChild<UI::VerticalLayout>();
    selectionField.SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN);

    const std::shared_ptr<Graphics::Font>& p_font = p_style->GetFont("Default-UI");

    UI::TextElement& fieldLabel = selectionField.EmplaceChild<UI::TextElement>();
    fieldLabel.SetFont(p_font)
        .SetText(p_font->CreateText(fieldName))
        .SetTextColor(glm::vec4(1.0F, 1.0F, 0.0F, 1.0F))
        .SetFixedSize(fieldLabel.GetTextSize())
        .SetLayoutMode(UI::LayoutMode::FIXED)
        .SetOrigin({0.5F, 0.5F})
        .SetRelativePosition({0.5F, 0.5F});

    UI::Radio& radio = selectionField.EmplaceChild<UI::Radio>();
    radio.SetStyle(p_style->GetRadioStyle("simple"))
        .SetOptions(options)
        .SetValueChangedCallback(std::move(callback))
        .SelectOption(selected_index)
        .SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN)
        .SetOrigin({0.5F, 0.5F})
        .SetRelativePosition({0.5F, 0.5F});
}


// Add slider
void AddSliderSelection(
    const std::shared_ptr<UI::Style>& p_style,
    UI::Element& parent,
    const std::string& fieldName, // name of field being modified
    const std::vector<std::string>& options, // possible options to select from
    size_t selected_index, // currently selected option
    SelectionChangeCallback_t callback // callback when selection changes
) {

    UI::VerticalLayout& selectionField = parent.EmplaceChild<UI::VerticalLayout>();
    selectionField.SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN);

    const std::shared_ptr<Graphics::Font>& p_font = p_style->GetFont("Default-UI");

    UI::TextElement& fieldLabel = selectionField.EmplaceChild<UI::TextElement>();
    fieldLabel.SetFont(p_font)
        .SetText(p_font->CreateText(fieldName))
        .SetTextColor(glm::vec4(1.0F, 1.0F, 0.0F, 1.0F))
        .SetFixedSize(fieldLabel.GetTextSize())
        .SetLayoutMode(UI::LayoutMode::FIXED)
        .SetOrigin({0.5F, 0.5F})
        .SetRelativePosition({0.5F, 0.5F});

    UI::Slider& slider = selectionField.EmplaceChild<UI::Slider>();
    slider.SetStyle(p_style->GetSliderStyle("simple"))
        .SetOptions(options)
        .SetValueChangedCallback(std::move(callback))
        .SelectOption(selected_index)
        .SetSliderState(UI::SliderState::ENABLED)
        .SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN);

}

void AddTextInputBox(const std::shared_ptr<UI::Style> &p_style, UI::Element &parent, const std::string &fieldName, const std::string &default_text, uint16_t max_characters, std::function<void (const std::string &)> callback) {

    UI::HorizontalLayout& inputField = parent.EmplaceChild<UI::HorizontalLayout>();
    inputField.SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN);
    const std::shared_ptr<Graphics::Font>& p_font = p_style->GetFont("Default-UI");
    UI::TextElement& fieldLabel = inputField.EmplaceChild<UI::TextElement>();
    fieldLabel.SetFont(p_font)
        .SetText(p_font->CreateText(fieldName))
        .SetTextColor(glm::vec4(1.0F, 1.0F, 0.0F, 1.0F))
        .SetFixedSize(fieldLabel.GetTextSize())
        .SetLayoutMode(UI::LayoutMode::FIXED)
        .SetOrigin({0.5F, 0.5F})
        .SetRelativePosition({0.5F, 0.5F});

    UI::TextInputBox& textInputBox = inputField.EmplaceChild<UI::TextInputBox>();
    textInputBox.SetStyle(p_style->GetTextInputBoxStyle("simple"))
        .SetDefaultText(default_text)
        .SetMaxCharacters(max_characters)
        .SetTextInputState(UI::TextInputState::ENABLED)
        .SetTextValueChangedCallback(std::move(callback))
        .SetLayoutMode(UI::LayoutMode::FIT_TO_CHILDREN);
}

}