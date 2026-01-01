/**
 * @file RadioStyle.hpp
 * @brief Styling for radio control groups (checkbox style + font).
 */

#pragma once

#include "graphics/Font.hpp"
#include "ui/CheckBoxStyle.hpp"

#include <memory>

namespace UI {

    /**
     * @class RadioStyle
     * @brief Holds shared resources used by `Radio` groups (checkbox style and font).
     */
    class RadioStyle {

        public:

            RadioStyle();

            /**
             * @brief Set the checkbox style used by radio controls.
             * @param p_style Shared pointer to a CheckBoxStyle.
             */
            void SetCheckboxStyle(std::shared_ptr<CheckBoxStyle> p_style);
            /**
             * @brief Get the configured checkbox style.
             * @return Shared pointer to the CheckBoxStyle.
             */
            const std::shared_ptr<CheckBoxStyle>& GetCheckboxStyle();
            /**
             * @brief Set the font used to render radio labels.
             * @param p_font Shared pointer to a Graphics::Font.
             */
            void SetFont(std::shared_ptr<Graphics::Font> p_font);
            /**
             * @brief Get the configured font.
             * @return Shared pointer to the Graphics::Font.
             */
            const std::shared_ptr<Graphics::Font>& GetFont();

        private:

            //! style for checkboxes used to implement radio selection
            std::shared_ptr<CheckBoxStyle> m_p_checkbox_style;

            //! font used for labelling the selection.
            std::shared_ptr<Graphics::Font> m_p_font;
    };
}