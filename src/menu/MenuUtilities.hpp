#pragma once

#include "ui/Button.hpp"
#include "ui/Element.hpp"
#include "ui/Style.hpp"
#include "ui/TextElement.hpp"
#include <functional>
#include <list>
#include <memory>

namespace Menu {

    using SelectionChangeCallback_t = std::function<void(size_t selection)>;

    // Add a background image
    UI::Element& AddBackground(
        const std::shared_ptr<UI::Style>& p_style,
        UI::Element& parent
    );

    // Add common UI button.
    UI::Button& AddButton(
        const std::shared_ptr<UI::Style>& p_style,
        UI::Element& parent,
        const std::string& text_label,
        UI::ButtonState state,
        UI::OnClickCallback callback = {});

    // Add text
    UI::TextElement& AddTextElement(
        const std::shared_ptr<UI::Style>& p_style,
        UI::Element& parent,
        const std::string& text,
        const glm::vec4& color,
        size_t max_characters);

    // Add Radio selection.
    void AddRadioSelection(
        const std::shared_ptr<UI::Style>& p_style,
        UI::Element& parent,
        const std::string& fieldName, // name of field being modified
        const std::vector<std::string>& options, // possible options to select from
        size_t selected_index, // currently selected option
        SelectionChangeCallback_t callback // callback when selection changes.
    );

    // Add slider
    void AddSliderSelection(
        const std::shared_ptr<UI::Style>& p_style,
        UI::Element& parent,
        const std::string& fieldName, // name of field being modified
        const std::vector<std::string>& options, // possible options to select from
        size_t selected_index, // currently selected option
        SelectionChangeCallback_t callback // callback when selection changes
    );

    // Add Text Input Box
    void AddTextInputBox(
        const std::shared_ptr<UI::Style>& p_style,
        UI::Element& parent,
        const std::string& fieldName, // name of field being modified
        const std::string& default_text, // default text to show when no text is entered
        uint16_t max_characters, // maximum number of characters allowed in the text box
        std::function<void(const std::string&)> callback // callback when text is changed
    );



}