#pragma once

#include "Element.hpp"
#include <glm/ext/vector_float4.hpp>
#include <string>

namespace UI {

    class TextInputBox : public Element {

        public:

            /**
             * @brief Constructor.
             */
            TextInputBox();

            TextInputBox& SetCurrentText();

        private:

            //! Currently displayed text
            std::string m_user_text;

            //! Color to use for current text.
            std::string m_user_text_color;

            //! Default text to show, when no text is supplied.
            std::string m_default_text;

            //! Color used to display default text.
            glm::vec4 m_default_text_color;

            //! maximum number of characters.
            uint16_t m_max_character_count;

            //! caret position
            uint16_t m_caret_pos;

            //! Highlight text if end pos > caret pos
            uint16_t m_select_end_pos;
    };
}