#pragma once

#include "ui/ButtonStyle.hpp"
#include <memory>

namespace UI {

    class DropDownStyle {

        public:

            DropDownStyle();

            void SetSelectionButtonStyle(std::shared_ptr<ButtonStyle> p_style);
            const std::shared_ptr<ButtonStyle>& GetSelectionButtonStyle();
            void SetOptionsButtonStyle(std::shared_ptr<ButtonStyle> p_options);
            const std::shared_ptr<ButtonStyle>& GetOptionsButtonStyle();

        private:

            //! style for drop down button. This is button that displays the currently selected value.
            std::shared_ptr<ButtonStyle> m_p_button_style;

            //! style for buttons in the options menu. This displays possible choices for the button
            std::shared_ptr<ButtonStyle> m_p_options_style;
    };
}