#include "MenuUtilities.hpp"

void Menu::AddButton(
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
}