/**
 * @file TextElement.hpp
 * @brief Declaration for a text UI element (renders font text).
 */

#pragma once

#include "Element.hpp"
#include "graphics/Font.hpp"
#include "glm/vec4.hpp"

namespace UI {

    /**
     * @class TextElement
     * @brief UI element that renders a single text object using a configured font.
     */
    class TextElement : public Element {

        public:

            /** @brief Default constructor. */
            TextElement();

            /**
                * @brief Set the font used to render the text.
                * @param p_font Shared pointer to a Graphics::Font.
                * @return Reference to this TextElement.
                */
            TextElement& SetFont(std::shared_ptr<Graphics::Font> p_font);

            /**
                * @brief Set the SDL text object to render.
                * @param p_text Shared pointer to an SDL text object.
                * @return Reference to this TextElement.
                */
            TextElement& SetText(std::shared_ptr<SDL::TTF::Text> p_text);

            /**
                * @brief Set the displayed text string (updates underlying SDL text object).
                * @param str The string to display.
                * @return Reference to this TextElement.
                */
            TextElement& SetTextString(const std::string& str);

            /**
                * @brief Set the text color.
                * @param color RGBA color vector.
                * @return Reference to this TextElement.
                */
            TextElement& SetTextColor(const glm::vec4& color);

            /** @brief Get the pixel size of the current text layout. */
            glm::vec2 GetTextSize() const;

            /**
                * @brief Update rendering entities for the text element.
                * @param registry ECS registry used for rendering components.
                * @param screenSize Screen resolution in pixels.
                * @param depth Draw order depth.
                */
            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) override;

            /** @brief Clear any rendering entities created for this element. */
            void ClearGraphics() override;

        private:
            std::shared_ptr<Graphics::Font> m_p_font;
            std::shared_ptr<SDL::TTF::Text> m_p_text;
            glm::vec4 m_color;
            ECS::Entity m_entity;
    };
}