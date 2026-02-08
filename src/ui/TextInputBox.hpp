#pragma once

#include "Element.hpp"
#include "NineSlice.hpp"
#include "TextElement.hpp"
#include "TextInputBoxStyle.hpp"
#include <cstdint>
#include <glm/ext/vector_float4.hpp>
#include <string>

namespace UI {

    using TextValueChangeCallback = std::function<void(const std::string&)>;

    class TextInputBox : public Element {

        public:

            /**
             * @brief Constructor.
             */
            TextInputBox();

            /**
             * @brief Set the style used for this text input box.
             * @param p_style Shared pointer to a TextInputBoxStyle.
             * @return Reference to this TextInputBox.
             */
            TextInputBox& SetStyle(std::shared_ptr<TextInputBoxStyle> p_style);

            /**
             * @brief Set the state of the text input box.
             * @param state New state to apply.
             */
             TextInputBox& SetTextInputState(TextInputState state);

            /**
             * @brief Set the default text to show when no text is entered.
             * @param text Default text to show.
             * @return Reference to this TextInputBox.
             */
            TextInputBox& SetDefaultText(const std::string& text);

            /**
             * @brief Set the maximum number of characters allowed in the text box.
             * @param max_chars Maximum character count.
             */
            TextInputBox& SetMaxCharacters(uint16_t max_chars);

            /**
             * @brief Set the callback invoked when the text value changes.
             * @param callback Function taking the new text string.
             */
            TextInputBox& SetTextValueChangedCallback(TextValueChangeCallback callback);

            /**
             * @brief Get whether this text input box is currently processing input.
             * @return True if the text input box is processing input, false otherwise.
             */
            bool IsTextInputEnabled() const override;

            /**
             * @brief Update Graphics entities for this text input box.
             * @param registry ECS registry used for rendering components.
             * @param screenSize The full screen resolution in pixels.
             * @param depth Draw order depth.
             */
            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) override;

            /**
             * @brief Replace test in the text box with new text.
             * @param text New text to set.
             */
            void SetTextString(const std::string& text);

            /**
             * @brief Insert text at the current caret position.
             * @note This updates the displayed text and moves the caret to the end of the inserted text.
             * @param text Text to insert.
             */
            void InsertText(const std::string& text);
        private:

            /**
             * @brief Handle keyboard input events for text editing.
             * @param event The keyboard event.
             */
            void HandleKeyboardInput(const SDL_KeyboardEvent& event);

            //! The style used for this text input box.
            std::shared_ptr<TextInputBoxStyle> m_p_style;

            //! Current state of the text input box.
            TextInputState m_state;

            //! Currently displayed text
            std::string m_user_text;

            //! Default text to show, when no text is supplied.
            std::string m_default_text;

            //! Maximum number of characters allowed in the text box.
            uint16_t m_max_characters;

            //! Pointer to the background nine-slice element.
            NineSlice* m_p_background;

            //! Pointer to the text element.
            TextElement* m_p_text_element;

            //! Whether the text input box is processing input.
            bool m_processing_input;

            //! Current caret position in the text.
            size_t m_caret_position;

            //! NineSlice used to render the caret.
            NineSlice* m_p_caret;

            //! Blink timer for the caret.
            float m_last_blink_toggle_time_sec;

            //! Callback invoked when the text value changes.
            TextValueChangeCallback m_text_value_change_callback;
    };
}