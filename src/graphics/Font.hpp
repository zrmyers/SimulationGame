#pragma once

#include "core/Engine.hpp"
#include "sdl/TTF.hpp"
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>

namespace Systems {
    class TextSystem;
}

namespace Graphics {

    class Font {

        public:

            Font(
                Core::AssetLoader& assetLoader,
                Systems::TextSystem& textSystem,
                const std::string& name,
                float pt_size,
                bool use_sdf,
                TTF_HorizontalAlignment alignment);

            std::shared_ptr<SDL::TTF::Text> CreateText(const std::string& text_str);

            bool GetSDF() const;

            //! @brief Get the maximum glyph size in pixels.
            glm::vec2 GetMaxGlyphSizePx() const;

            //! @brief Get Glyph metrics for a given character.
            void GetGlyphMetrics(uint16_t ascii, glm::vec2& min, glm::vec2& max, float& advance);

        private:

            //! @brief Calculate glyph sizes and determine maximum possible glyph
            //! size for this font.
            void MeasureGlyphs();

            Systems::TextSystem* m_p_text_system {nullptr};
            SDL::TTF::Font m_font;

            glm::vec2 m_max_glyph_size_px;
    };
}