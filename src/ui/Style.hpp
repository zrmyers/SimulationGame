#pragma once

#include "CheckBoxStyle.hpp"
#include "DropDownStyle.hpp"
#include "NineSliceStyle.hpp"
#include "ButtonStyle.hpp"
#include "core/Engine.hpp"
#include "graphics/Texture2D.hpp"
#include "ui/RadioStyle.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace UI {


    class Style {

        public:

            static Style Load(Core::Engine& engine, const std::string& filename);

            Style() = default;

            void SetFont(const std::string& font_id, std::shared_ptr<Graphics::Font> p_font);
            std::shared_ptr<Graphics::Font>& GetFont(const std::string& font_id);

            void SetNineSliceStyle(const std::string& style_id, std::shared_ptr<NineSliceStyle>&& style);
            std::shared_ptr<NineSliceStyle>& GetNineSliceStyle(const std::string& nineslice_id);

            void SetButtonStyle(const std::string& button_id, std::shared_ptr<ButtonStyle>&& style);
            std::shared_ptr<ButtonStyle>& GetButtonStyle(const std::string& button_id);

            void SetCheckBoxStyle(const std::string& checkbox_id, std::shared_ptr<CheckBoxStyle>&& style);
            std::shared_ptr<CheckBoxStyle>& GetCheckBoxStyle(const std::string& checkbox_id);

            void SetDropDownStyle(const std::string& dropdown_id, std::shared_ptr<DropDownStyle>&& style);
            std::shared_ptr<DropDownStyle>& GetDropDownStyle(const std::string& dropdown_id);

            void SetRadioStyle(const std::string& radio_id, std::shared_ptr<RadioStyle>&& style);
            std::shared_ptr<RadioStyle>& GetRadioStyle(const std::string& radio_id);

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
    };

}