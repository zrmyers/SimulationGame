#pragma once

#include "Renderable.hpp"
#include "sdl/TTF.hpp"
#include <string>
#include <glm/vec4.hpp>
#include <memory>

namespace Components {

    struct Text {

        //! The color of the text.
        glm::vec4 m_color;

        //! The font with which to create the text.
        std::shared_ptr<SDL::TTF::Font> m_p_font;

        //! The text object. created by Text system
        std::shared_ptr<SDL::TTF::Text> m_p_text;

        //! Which layer the text is rendered to.
        RenderLayer m_layer{};

        //! Draw Order.
        uint32_t m_draw_order{0U};

        Text() : m_color(1.0F, 1.0F, 1.0F, 1.0F), m_layer(RenderLayer::LAYER_NONE) { };
    };
}
