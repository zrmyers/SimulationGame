#pragma once
#include <functional>
#include "Element.hpp"
#include "RadioStyle.hpp"
#include "ui/CheckBox.hpp"

namespace UI {

    using RadioValueChangeCallback_t = std::function<void(size_t)>;

    class Radio : public UI::Element {

        public:
            Radio();

            Radio& SetStyle(const std::shared_ptr<RadioStyle>& p_style);
            Radio& SelectOption(size_t select);
            Radio& SetOptions(std::vector<std::string> options);
            Radio& SetValueChangedCallback(RadioValueChangeCallback_t callback);

            //! Update the currently displayed graphics.
            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) override;

        private:

            //! The style to use for spawned buttons.
            std::shared_ptr<RadioStyle> m_p_style;

            //! The list of options
            std::vector<std::string> m_options;

            //! The currently selected position.
            size_t m_selected;

            //! Pointers to selection checkboxes
            std::vector<UI::CheckBox*> m_p_buttons;
            UI::CheckBox* m_p_selected;

            //! The callback function when drop down value changes.
            RadioValueChangeCallback_t m_value_change_callback;

    };
}