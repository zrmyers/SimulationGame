#include "Font.hpp"
#include "core/AssetLoader.hpp"
#include "sdl/TTF.hpp"
#include "systems/TextSystem.hpp"
#include <SDL3_ttf/SDL_ttf.h>

Graphics::Font::Font(
    Core::AssetLoader& assetLoader,
    Systems::TextSystem& textSystem,
    const std::string& name,
    float pt_size,
    bool use_sdf,
    TTF_HorizontalAlignment alignment)
    : m_p_text_system(&textSystem) {

    m_font = SDL::TTF::Font(assetLoader.GetFontDir() + "/" + name, pt_size);
    m_font.SetSDF(use_sdf);

    m_font.SetHorizontalAlignment(alignment);
}

std::shared_ptr<SDL::TTF::Text> Graphics::Font::CreateText(const std::string& text_str) {

    return std::make_shared<SDL::TTF::Text>(m_p_text_system->GetTextEngine(), m_font, text_str);
}

bool Graphics::Font::GetSDF() const {
    return m_font.GetSDF();
}