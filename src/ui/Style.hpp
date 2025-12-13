#pragma once

#include "NineSliceStyle.hpp"
#include "ButtonStyle.hpp"

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

        private:

            static TTF_HorizontalAlignment ParseAlignment(const std::string& asString);
            static glm::vec4 ParseColor(nlohmann::json& colorData);

            //! Set of fonts that are used in style.
            std::unordered_map<std::string, std::shared_ptr<Graphics::Font>> m_fonts;

            //! Set of Nine-Slice-Elements used in style.
            std::unordered_map<std::string, std::shared_ptr<NineSliceStyle>> m_nine_slice_styles;

            //! Set of button styles
            std::unordered_map<std::string, std::shared_ptr<ButtonStyle>> m_button_style;
    };

}