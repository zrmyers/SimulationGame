/**
 * @file Style.hpp
 * @brief Centralized style repository for UI widgets (fonts, nine-slice and widget styles).
 */

#pragma once

#include "CheckBoxStyle.hpp"
#include "DropDownStyle.hpp"
#include "NineSliceStyle.hpp"
#include "ButtonStyle.hpp"
#include "SliderStyle.hpp"
#include "core/Engine.hpp"
#include "graphics/Texture2D.hpp"
#include "ui/RadioStyle.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace UI {


    /**
     * @class Style
     * @brief Holds shared style resources used across UI widgets.
     */
    class Style {

        public:

            /**
             * @brief Load a style definition from JSON data.
             * @param engine Engine used to resolve resources.
             * @param filename Path to the style JSON file.
             * @return Populated Style instance.
             */
            static Style Load(Core::Engine& engine, const std::string& filename);

            Style() = default;

            /**
             * @brief Register a font resource under an identifier.
             * @param font_id Identifier for the font.
             * @param p_font Shared pointer to the font resource.
             */
            void SetFont(const std::string& font_id, std::shared_ptr<Graphics::Font> p_font);
            /**
             * @brief Retrieve a previously registered font by id.
             * @param font_id Identifier of the font to retrieve.
             * @return Shared pointer to the font resource.
             */
            std::shared_ptr<Graphics::Font>& GetFont(const std::string& font_id);

            /**
             * @brief Register a nine-slice style under an identifier.
             * @param style_id Identifier for the nine-slice style.
             * @param style Unique pointer to the style being stored.
             */
            void SetNineSliceStyle(const std::string& style_id, std::shared_ptr<NineSliceStyle>&& style);
            /**
             * @brief Retrieve a nine-slice style by id.
             * @param nineslice_id Identifier of the style to retrieve.
             * @return Shared pointer to the nine-slice style.
             */
            std::shared_ptr<NineSliceStyle>& GetNineSliceStyle(const std::string& nineslice_id);

            /**
             * @brief Register a button style under an identifier.
             * @param button_id Identifier for the button style.
             * @param style Unique pointer to the ButtonStyle.
             */
            void SetButtonStyle(const std::string& button_id, std::shared_ptr<ButtonStyle>&& style);
            /**
             * @brief Retrieve a button style by id.
             * @param button_id Identifier of the button style.
             * @return Shared pointer to the ButtonStyle.
             */
            std::shared_ptr<ButtonStyle>& GetButtonStyle(const std::string& button_id);

            /**
             * @brief Register a checkbox style under an identifier.
             * @param checkbox_id Identifier for the checkbox style.
             * @param style Unique pointer to the CheckBoxStyle.
             */
            void SetCheckBoxStyle(const std::string& checkbox_id, std::shared_ptr<CheckBoxStyle>&& style);
            /**
             * @brief Retrieve a checkbox style by id.
             * @param checkbox_id Identifier of the checkbox style.
             * @return Shared pointer to the CheckBoxStyle.
             */
            std::shared_ptr<CheckBoxStyle>& GetCheckBoxStyle(const std::string& checkbox_id);

            /**
             * @brief Register a drop-down style under an identifier.
             * @param dropdown_id Identifier for the drop-down style.
             * @param style Unique pointer to the DropDownStyle.
             */
            void SetDropDownStyle(const std::string& dropdown_id, std::shared_ptr<DropDownStyle>&& style);
            /**
             * @brief Retrieve a drop-down style by id.
             * @param dropdown_id Identifier of the drop-down style.
             * @return Shared pointer to the DropDownStyle.
             */
            std::shared_ptr<DropDownStyle>& GetDropDownStyle(const std::string& dropdown_id);

            /**
             * @brief Register a radio style under an identifier.
             * @param radio_id Identifier for the radio style.
             * @param style Unique pointer to the RadioStyle.
             */
            void SetRadioStyle(const std::string& radio_id, std::shared_ptr<RadioStyle>&& style);
            /**
             * @brief Retrieve a radio style by id.
             * @param radio_id Identifier of the radio style.
             * @return Shared pointer to the RadioStyle.
             */
            std::shared_ptr<RadioStyle>& GetRadioStyle(const std::string& radio_id);

            /**
             * @brief Register a slider style under an identifier.
             * @param slider_id Identifier for the slider style.
             * @param style Unique pointer to the SliderStyle.
             */
            void SetSliderStyle(const std::string& slider_id, std::shared_ptr<SliderStyle>&& style);
            /**
             * @brief Retrieve a slider style by id.
             * @param slider_id Identifier of the slider style.
             * @return Shared pointer to the SliderStyle.
             */
            std::shared_ptr<SliderStyle>& GetSliderStyle(const std::string& slider_id);

            /**
             * @brief Set a text input box style by ID.
             * @param box_id Identifier for the text input box style.
             */
             void SetTextInputBoxStyle(const std::string& box_id, std::shared_ptr<NineSliceStyle> p_style);

             

        private:

            static TTF_HorizontalAlignment ParseAlignment(const std::string& asString);
            static glm::vec4 ParseColor(nlohmann::json& colorData);

            static std::shared_ptr<Graphics::Texture2D> LoadImage(Core::Engine& engine, const std::string& name);

            //! Set of fonts that are used in style.
            std::unordered_map<std::string, std::shared_ptr<Graphics::Font>> m_fonts;

            //! Set of Nine-Slice-Elements used in style.
            std::unordered_map<std::string, std::shared_ptr<NineSliceStyle>> m_nine_slice_styles;

            //! Set of button styles
            std::unordered_map<std::string, std::shared_ptr<ButtonStyle>> m_button_style;

            //! Set of checkbox styles
            std::unordered_map<std::string, std::shared_ptr<CheckBoxStyle>> m_checkbox_style;

            //! Set of dropdown button styles
            std::unordered_map<std::string, std::shared_ptr<DropDownStyle>> m_dropdown_style;

            //! Set of radio button styles
            std::unordered_map<std::string, std::shared_ptr<RadioStyle>> m_radio_styles;

            //! Set of slider styles
            std::unordered_map<std::string, std::shared_ptr<SliderStyle>> m_slider_styles;
    };

}