#pragma once

#include "CheckBoxStyle.hpp"
#include "Element.hpp"
#include "ui/ImageElement.hpp"
#include <functional>
#include <memory>

namespace UI {

    // Callback when the check box state is changed. The input parameter indicates whether the checkbox is
    // on (true) or off (false)
    using CheckBoxStateCallback_t = std::function<void(bool)>;

    // Displays as either on or off.
    class CheckBox : public Element {

        public:
            CheckBox();

            CheckBox& SetStyle(std::shared_ptr<CheckBoxStyle> p_style);

            CheckBox& SetCheckBoxStateCallback(CheckBoxStateCallback_t callback);

            CheckBox& SetCheckBoxState(CheckBoxState state);

            //! Update the currently displayed graphics.
            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) override;

        private:

            std::shared_ptr<CheckBoxStyle> m_p_style;
            CheckBoxStateCallback_t m_state_callback;
            ImageElement* m_p_image_element;
            CheckBoxState m_current_state;
    };
}