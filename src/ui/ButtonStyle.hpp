/**
 * @file ButtonStyle.hpp
 * @brief Style information for buttons (fonts, frames, text colors).
 */

#pragma once

#include "graphics/Font.hpp"
#include "NineSliceStyle.hpp"

namespace UI {

    /**
     * @enum ButtonState
     * @brief Visual and interaction state for buttons.
     */
    enum class ButtonState : int8_t {
        UNKNOWN = -1,
        DISABLED,
        ENABLED, // button can be focused, selected, and activated
        FOCUSED, // when mouse hover is detected.
        ACTIVATED, // processing click
        SELECTED, // button is selected
    };

    static const constexpr size_t NUM_BUTTON_STATES = 5U;

    /**
     * @class ButtonStyle
     * @brief Encapsulates styling for buttons: font, nine-slice frame styles and text colors.
     */
    class ButtonStyle {

        public:

            ButtonStyle();

            /**
             * @brief Set the font used for button text.
             * @param p_font Shared pointer to a Graphics::Font instance.
             */
            void SetFont(std::shared_ptr<Graphics::Font> p_font);
            /**
             * @brief Get the configured font for button text.
             * @return Shared pointer to the font resource.
             */
            const std::shared_ptr<Graphics::Font>& GetFont();
            /**
             * @brief Set the nine-slice frame style for a given button state.
             * @param state ButtonState to associate the style with.
             * @param p_style Shared pointer to a NineSliceStyle.
             */
            void SetNineSliceStyle(ButtonState state, std::shared_ptr<NineSliceStyle> p_style);
            /**
             * @brief Get the nine-slice style for a specific button state.
             * @param state ButtonState to query.
             * @return Shared pointer to the corresponding NineSliceStyle.
             */
            const std::shared_ptr<NineSliceStyle>& GetNineSliceStyle(ButtonState state);
            /**
             * @brief Set the text color for the given button state.
             * @param state ButtonState to modify.
             * @param color RGBA color vector.
             */
            void SetTextColor(ButtonState state, glm::vec4 color);
            /**
             * @brief Get the text color for the given button state.
             * @param state ButtonState to query.
             * @return RGBA color vector for the state's text.
             */
            const glm::vec4& GetTextColor(ButtonState state);

        private:

            std::shared_ptr<Graphics::Font> m_p_text_font;
            std::array<std::shared_ptr<NineSliceStyle>, NUM_BUTTON_STATES> m_frame_styles;
            std::array<glm::vec4, NUM_BUTTON_STATES> m_text_colors {};
    };
}