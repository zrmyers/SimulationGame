#pragma once

#include "graphics/Font.hpp"
#include "ui/CheckBoxStyle.hpp"

#include <memory>

namespace UI {

    class RadioStyle {

        public:

            RadioStyle();

            void SetCheckboxStyle(std::shared_ptr<CheckBoxStyle> p_style);
            const std::shared_ptr<CheckBoxStyle>& GetCheckboxStyle();
            void SetFont(std::shared_ptr<Graphics::Font> p_font);
            const std::shared_ptr<Graphics::Font>& GetFont();

        private:

            //! style for checkboxes used to implement radio selection
            std::shared_ptr<CheckBoxStyle> m_p_checkbox_style;

            //! font used for labelling the selection.
            std::shared_ptr<Graphics::Font> m_p_font;
    };
}