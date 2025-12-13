#pragma once

#include "graphics/Font.hpp"
#include "NineSliceStyle.hpp"

namespace UI {

    enum class ButtonState : int8_t {
        UNKNOWN = -1,
        DISABLED,
        ENABLED, // button can be focused, selected, and activated
        FOCUSED, // when mouse hover is detected.
        ACTIVATED, // processing click
        SELECTED, // button is selected
    };

    static const constexpr size_t NUM_BUTTON_STATES = 5U;

    class ButtonStyle {

        public:

            ButtonStyle();

            void SetFont(std::shared_ptr<Graphics::Font> p_font);
            const std::shared_ptr<Graphics::Font>& GetFont();
            void SetNineSliceStyle(ButtonState state, std::shared_ptr<NineSliceStyle> p_style);
            const std::shared_ptr<NineSliceStyle>& GetNineSliceStyle(ButtonState state);
            void SetTextColor(ButtonState state, glm::vec4 color);
            const glm::vec4& GetTextColor(ButtonState state);

        private:

            std::shared_ptr<Graphics::Font> m_p_text_font;
            std::array<std::shared_ptr<NineSliceStyle>, NUM_BUTTON_STATES> m_frame_styles;
            std::array<glm::vec4, NUM_BUTTON_STATES> m_text_colors {};
    };
}