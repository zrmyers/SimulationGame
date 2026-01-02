#include "Font.hpp"
#include "core/AssetLoader.hpp"
#include "sdl/TTF.hpp"
#include "systems/TextSystem.hpp"
#include <SDL3_ttf/SDL_ttf.h>
#include <sstream>

Graphics::Font::Font(
    Core::AssetLoader& assetLoader,
    Systems::TextSystem& textSystem,
    const std::string& name,
    float pt_size,
    bool use_sdf,
    TTF_HorizontalAlignment alignment)
    : m_p_text_system(&textSystem)
    , m_max_glyph_size_px(0.0F, 0.0F) {

    m_font = SDL::TTF::Font(assetLoader.GetFontDir() + "/" + name, pt_size);
    m_font.SetSDF(use_sdf);

    m_font.SetHorizontalAlignment(alignment);

    MeasureGlyphs();
}

std::shared_ptr<SDL::TTF::Text> Graphics::Font::CreateText(const std::string& text_str) {

    return std::make_shared<SDL::TTF::Text>(m_p_text_system->GetTextEngine(), m_font, text_str);
}

bool Graphics::Font::GetSDF() const {
    return m_font.GetSDF();
}

glm::vec2 Graphics::Font::GetMaxGlyphSizePx() const {
    return m_max_glyph_size_px;
}

void Graphics::Font::MeasureGlyphs() {

    // Preload ASCII glyphs 32-126
    for (uint16_t ascii = 32U; ascii <= 255U; ascii++) {

        glm::vec2 min(0.0F);
        glm::vec2 max(0.0F);
        float advance = 0.0F;
        m_font.GetGlyphMetrics(ascii, min, max, advance);

        glm::vec2 glyph_size = max - min;
        m_max_glyph_size_px.x = std::max(m_max_glyph_size_px.x, advance);
        m_max_glyph_size_px.y = std::max(m_max_glyph_size_px.y, glyph_size.y);
    }
}