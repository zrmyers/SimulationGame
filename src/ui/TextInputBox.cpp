/**
 * @file TextInputBox.cpp
 * @brief Implementation for `UI::TextInputBox` — a text input UI element.
 */

#include "TextInputBox.hpp"

#include "NineSlice.hpp"
#include "TextElement.hpp"
#include "core/Engine.hpp"
#include "core/Logger.hpp"
#include "graphics/Font.hpp"
#include "sdl/SDL.hpp"
#include "systems/RenderSystem.hpp"
#include "ui/Element.hpp"
#include "ui/TextInputBoxStyle.hpp"
#include <SDL3/SDL_events.h>
#include <memory>
#include <string>

namespace UI {

TextInputBox::TextInputBox()
    : m_state(TextInputState::UNKNOWN)
    , m_max_characters(20U) // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    , m_p_background(nullptr)
    , m_p_text_element(nullptr)
    , m_processing_input(false)
    , m_caret_position(0U)
    , m_p_caret(nullptr)
    , m_last_blink_toggle_time_sec(0.0F)
{
    SetHoverEnterCallback([this](){
        switch (m_state) {

            case UI::TextInputState::ENABLED:
                this->SetTextInputState(TextInputState::FOCUSED);
                break;

            case UI::TextInputState::FOCUSED:
            default:
                break;
        }
    });
    SetHoverExitCallback([this]() {
        if (!m_processing_input) {
            switch (m_state) {

                case UI::TextInputState::FOCUSED:
                    this->SetTextInputState(TextInputState::ENABLED);
                    break;

                case UI::TextInputState::ENABLED:
                default:
                    break;
            }
        }
    });

    SetMouseButtonReleaseCallback([this](UI::MouseButtonID button){
        // Stop processing text input
        if (button == UI::MouseButtonID::MOUSE_LEFT && (m_state == TextInputState::FOCUSED)) {
            m_processing_input = true;
        }
    });

    SetTextInputCallback([this](const std::string& text){
        InsertText(text);
    });

    SetKeyboardInputCallback([this](const SDL_KeyboardEvent& event) {
        this->HandleKeyboardInput(event);
    });
}

TextInputBox& TextInputBox::SetStyle(std::shared_ptr<TextInputBoxStyle> p_style) {
    m_p_style = std::move(p_style);
    return *this;
}

TextInputBox& TextInputBox::SetDefaultText(const std::string& text) {
    m_default_text = text;
    return *this;
}

TextInputBox& TextInputBox::SetMaxCharacters(uint16_t max_chars) {
    m_max_characters = max_chars;
    return *this;
}

TextInputBox& TextInputBox::SetTextValueChangedCallback(TextValueChangeCallback callback) {
    m_text_value_change_callback = std::move(callback);
    return *this;
}

TextInputBox& TextInputBox::SetTextInputState(TextInputState state) {

    if (m_state != state && (m_p_style != nullptr)) {

        if (m_state == TextInputState::UNKNOWN) {

            // create background nine-slice
            NineSlice& background = EmplaceChild<UI::NineSlice>();
            background.SetLayoutMode(LayoutMode::FIT_TO_CHILDREN);
            m_p_background = &background;

            // create text element
            std::shared_ptr<Graphics::Font> p_font = m_p_style->GetTextFont();

            glm::vec2 maxTextSize = p_font->GetMaxGlyphSizePx();
            maxTextSize.x *= static_cast<float>(m_max_characters);

            TextElement& textElement = background.EmplaceChild<UI::TextElement>();
            textElement.SetFont(p_font)
                .SetTextColor(m_p_style->GetDefaultTextColor())
                .SetText(p_font->CreateText(m_default_text))
                .SetLayoutMode(LayoutMode::FIXED)
                .SetFixedSize(maxTextSize)
                .SetOrigin({0.5F, 0.5F})
                .SetRelativePosition({0.5F, 0.5F})
                .SetOffsetPosition({0.0F, maxTextSize.y / 2.0F - 3.0F});

            NineSlice& caret = background.EmplaceChild<UI::NineSlice>();
            caret.SetStyle(m_p_style->GetCaretStyle())
                .SetLayoutMode(LayoutMode::FIXED)
                .SetFixedSize({9.0F, maxTextSize.y})
                .SetOffsetPosition({0.0F, 0.0F})
                .SetOrigin({0.0F, 0.5F})
                .SetRelativePosition({0.0F, 0.5F})
                .SetOffsetPosition({-5.0F, 0.0F}); // offset slightly to the left of text.
            m_p_caret = &caret;

            m_p_caret->SetVisible(false);

            m_p_text_element = &textElement;
        }

        // update nine-slice style
        m_p_background->SetStyle(m_p_style->GetBoxStyle(state));

        m_state = state;
    }
    return *this;
}

void TextInputBox::UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) {

    // Get the window for text input
    SDL::Window& window = registry.GetSystem<Systems::RenderSystem>().GetWindow();
    bool textInputActive = window.IsTextInputActive();
    Core::Engine& engine = Core::Engine::GetInstance();

    if (m_processing_input && !textInputActive) {

        // Start SDL text input
        window.StartTextInput();

        m_p_caret->SetVisible(true);
        m_last_blink_toggle_time_sec = engine.GetElapsedTimeSec();
    }
    else if (!m_processing_input && textInputActive) {

        // Stop SDL text input
        window.StopTextInput();

        m_p_caret->SetVisible(false);
    }

    // Handle caret blinking
    if (m_processing_input) {

        // Determine if click has occurred outside of the text input box.
        // Additionally, need to handle keyboard events for moving caret, and deleting text.
        const std::vector<SDL_Event>& events = engine.GetEvents();
        for (const SDL_Event& event : events) {
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                glm::vec2 mousePos(static_cast<float>(event.button.x), static_cast<float>(event.button.y));
                if (!CheckCollision(mousePos)) {
                    // Clicked outside text input box, stop processing input
                    m_processing_input = false;
                    SetTextInputState(TextInputState::ENABLED);
                    break;
                }
            }
        }

        if (m_processing_input) {
            float currentTimeSec = engine.GetElapsedTimeSec();
            if (currentTimeSec - m_last_blink_toggle_time_sec >= 0.5F) {
                // toggle caret visibility
                bool isVisible = m_p_caret->GetVisible();
                m_p_caret->SetVisible(!isVisible);
                m_last_blink_toggle_time_sec = currentTimeSec;
            }
        }
    }

    // Delegate rendering to child elements (background + text)
    depth++;
    for (auto& p_child : GetChildren()) {
        p_child->UpdateGraphics(registry, screenSize, depth);
    }
}

void TextInputBox::InsertText(const std::string& text) {
    if (m_p_text_element != nullptr) {
        if (m_user_text.length() + text.length() <= m_max_characters) {

            std::stringstream newText;
            newText << m_user_text.substr(0, m_caret_position);
            newText << text;
            newText << m_user_text.substr(m_caret_position);

            // Update the user text
            m_user_text = newText.str();
            m_p_text_element->SetTextString(m_user_text);

            // Move caret position to end of inserted text
            m_caret_position += text.length();

            // Update caret display position
            float caretOffsetX = 0.0F;
            if (m_caret_position > 0) {

                glm::vec2 stringSize = m_p_text_element->MeasureStringSize(m_user_text.substr(0, m_caret_position));
                caretOffsetX = stringSize.x;
            }
            m_p_caret->SetOffsetPosition({caretOffsetX - 5.0F, 0.0F});

            if (m_text_value_change_callback) {
                m_text_value_change_callback(m_user_text);
            }
        }
    }
}


void TextInputBox::HandleKeyboardInput(const SDL_KeyboardEvent& event) { // NOLINT

    if (m_processing_input) {
        if (event.key == SDLK_BACKSPACE && event.type == SDL_EVENT_KEY_DOWN) {
            // Handle backspace
            if (m_caret_position > 0 && !m_user_text.empty()) {
                std::stringstream newText;
                newText << m_user_text.substr(0, m_caret_position - 1);
                newText << m_user_text.substr(m_caret_position);

                // Update the user text
                m_user_text = newText.str();

                if (m_user_text.empty()) {
                    m_p_text_element->SetTextString(m_default_text);
                }
                else {
                    m_p_text_element->SetTextString(m_user_text);
                }

                // Move caret position back
                m_caret_position--;

                // Update caret display position
                float caretOffsetX = 0.0F;
                if (m_caret_position > 0) {

                    glm::vec2 stringSize = m_p_text_element->MeasureStringSize(
                        m_user_text.substr(0, m_caret_position));
                    caretOffsetX = stringSize.x;
                }
                m_p_caret->SetOffsetPosition({caretOffsetX - 5.0F, 0.0F});
                if (m_text_value_change_callback) {
                    m_text_value_change_callback(m_user_text);
                }
            }
        }
        else if (event.key == SDLK_LEFT && event.type == SDL_EVENT_KEY_DOWN) {
            // Move caret left
            if (m_caret_position > 0) {
                m_caret_position--;

                // Update caret display position
                float caretOffsetX = 0.0F;
                if (m_caret_position > 0) {

                    glm::vec2 stringSize = m_p_text_element->MeasureStringSize(
                        m_user_text.substr(0, m_caret_position));
                    caretOffsetX = stringSize.x;
                }
                m_p_caret->SetOffsetPosition({caretOffsetX - 5.0F, 0.0F});
            }
        }
        else if (event.key == SDLK_RIGHT && event.type == SDL_EVENT_KEY_DOWN) {
            // Move caret right
            if (m_caret_position < m_user_text.length()) {
                m_caret_position++;

                // Update caret display position
                float caretOffsetX = 0.0F;
                if (m_caret_position > 0) {

                    glm::vec2 stringSize = m_p_text_element->MeasureStringSize(
                        m_user_text.substr(0, m_caret_position));
                    caretOffsetX = stringSize.x;
                }
                m_p_caret->SetOffsetPosition({caretOffsetX - 5.0F, 0.0F});
            }
        }
    }
}

} // namespace UI