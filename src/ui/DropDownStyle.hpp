/**
 * @file DropDownStyle.hpp
 * @brief Styles for drop-down widgets (selection button and options list buttons).
 */

#pragma once

#include "ui/ButtonStyle.hpp"
#include <memory>

namespace UI {

    /**
     * @class DropDownStyle
     * @brief Holds styles used by a DropDown: the selection button style and option button style.
     */
    class DropDownStyle {

        public:

            DropDownStyle();

            /**
             * @brief Set the style used for the visible selection button.
             * @param p_style Shared pointer to a ButtonStyle.
             */
            void SetSelectionButtonStyle(std::shared_ptr<ButtonStyle> p_style);
            /**
             * @brief Get the style used for the visible selection button.
             * @return Shared pointer to the ButtonStyle.
             */
            const std::shared_ptr<ButtonStyle>& GetSelectionButtonStyle();
            /**
             * @brief Set the style used for option buttons inside the dropdown.
             * @param p_options Shared pointer to a ButtonStyle for options.
             */
            void SetOptionsButtonStyle(std::shared_ptr<ButtonStyle> p_options);
            /**
             * @brief Get the style used for option buttons.
             * @return Shared pointer to the ButtonStyle for options.
             */
            const std::shared_ptr<ButtonStyle>& GetOptionsButtonStyle();

        private:

            //! style for drop down button. This is button that displays the currently selected value.
            std::shared_ptr<ButtonStyle> m_p_button_style;

            //! style for buttons in the options menu. This displays possible choices for the button
            std::shared_ptr<ButtonStyle> m_p_options_style;
    };
}