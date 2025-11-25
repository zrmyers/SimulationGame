#pragma once

#include "sdl/TTF.hpp"
#include <string>
#include <glm/vec4.hpp>
#include <memory>

namespace Components {

    struct Text {

        //! The formatted string to render as text.
        std::string m_string;

        //! The color of the text.
        glm::vec4 m_color;

        //! The font with which to create the text.
        std::shared_ptr<SDL::TTF::Font> m_p_font;

        //! The text object.
        std::shared_ptr<SDL::TTF::Text> m_p_text;

        Text() : m_color(1.0F, 1.0F, 1.0F, 1.0F) { };
    };
}
