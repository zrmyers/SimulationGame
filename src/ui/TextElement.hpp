#pragma once

#include "Element.hpp"
#include "graphics/Font.hpp"
#include "glm/vec4.hpp"

namespace UI {

    //! An element that contains a single image.
    class TextElement : public Element {

        public:

            TextElement();

            //! Sets the font for the font element.
            TextElement& SetFont(std::shared_ptr<Graphics::Font> p_font);
            TextElement& SetText(std::shared_ptr<SDL::TTF::Text> p_text);

            //! sets the text string
            TextElement& SetTextString(const std::string& str);
            TextElement& SetTextColor(const glm::vec4& color);

            //! Get the pixel size of characters in the text string.
            glm::vec2 GetTextSize() const;

            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) override;
        private:

            std::shared_ptr<Graphics::Font> m_p_font;
            std::shared_ptr<SDL::TTF::Text> m_p_text;

            glm::vec4 m_color;
            ECS::Entity m_entity;
    };
}