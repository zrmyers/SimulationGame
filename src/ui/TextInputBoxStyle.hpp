#pragma once

#include "NineSliceStyle.hpp"
#include "graphics/Font.hpp"
#include <memory>

namespace UI {

    enum class TextInputState : uint8_t {
        UNKNOWN = 0,
        ENABLED,
        FOCUSED
    };

    class TextInputBoxStyle {

        public:

            TextInputBoxStyle();

            void SetBoxStyle(TextInputState state, std::shared_ptr<UI::NineSliceStyle> style);
            std::shared_ptr<UI::NineSliceStyle> GetBoxStyle(TextInputState state) const;

            void SetTextFont(std::shared_ptr<Graphics::Font> font);
            std::shared_ptr<Graphics::Font> GetTextFont() const;

            void SetDefaultTextColor(const glm::vec4& color);
            glm::vec4 GetDefaultTextColor() const;

            void SetNormalTextColor(const glm::vec4& color);
            glm::vec4 GetNormalTextColor() const;

        private:

            std::unordered_map<TextInputState, std::shared_ptr<UI::NineSliceStyle>> m_p_box_style;
            std::shared_ptr<Graphics::Font> m_p_text_font;
            glm::vec4 m_default_text_color;
            glm::vec4 m_normal_text_color;
    };
}