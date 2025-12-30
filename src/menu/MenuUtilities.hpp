#pragma once

#include "ui/Button.hpp"
#include "ui/Element.hpp"
#include "ui/Style.hpp"
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
    void AddButton(
        const std::shared_ptr<UI::Style>& p_style,
        UI::Element& parent,
        const std::string& text_label,
        UI::ButtonState state,
        UI::OnClickCallback callback = {});

    // Add Radio selection.
    void AddRadioSelection(
        const std::shared_ptr<UI::Style>& p_style,
        UI::Element& parent,
        const std::string& fieldName, // name of field being modified
        const std::vector<std::string>& options, // possible options to select from
        size_t selected_index, // currently selected option
        SelectionChangeCallback_t callback // callback when selection changes.
    );

}