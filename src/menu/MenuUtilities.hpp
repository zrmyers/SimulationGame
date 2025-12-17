#pragma once

#include "ui/Button.hpp"
#include "ui/Element.hpp"
#include "ui/Style.hpp"

namespace Menu {

    // Add common UI button.
    void AddButton(
        const std::shared_ptr<UI::Style>& p_style,
        UI::Element& parent,
        const std::string& text_label,
        UI::ButtonState state,
        UI::OnClickCallback callback = {});

}