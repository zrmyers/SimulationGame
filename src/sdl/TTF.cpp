#include "TTF.hpp"
#include "sdl/SDL.hpp"
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdint>
#include <utility>


//----------------------------------------------------------------------------------------------------------------------
// TTF Context

SDL::TTF::Context::Context() {
    if(!TTF_Init()) {
        throw SDL::Error("TTF_Init() failed!");
    }
}

SDL::TTF::Context::~Context() {
    TTF_Quit();
}

//----------------------------------------------------------------------------------------------------------------------
// TTF Font

SDL::TTF::Font::Font()
    : m_p_font(nullptr) {
}

SDL::TTF::Font::Font(const std::string& font_filename, float ptsize)
    : m_p_font(TTF_OpenFont(font_filename.c_str(), ptsize)) {
    if (m_p_font == nullptr) {
        throw SDL::Error("TTF_OpenFont() failed!");
    }
}

SDL::TTF::Font::Font(Font&& other) noexcept
    : m_p_font(other.m_p_font) {
    other.m_p_font = nullptr;
}

SDL::TTF::Font& SDL::TTF::Font::operator=(SDL::TTF::Font&& other) noexcept {
    std::swap(m_p_font, other.m_p_font);

    return *this;
}

SDL::TTF::Font::~Font() {

    if (m_p_font != nullptr) {
        TTF_CloseFont(m_p_font);
    }
}

TTF_Font* SDL::TTF::Font::Get() {
    return m_p_font;
}

void SDL::TTF::Font::SetSDF(bool use_sdf) {
    if(!TTF_SetFontSDF(m_p_font, use_sdf)) {
        throw SDL::Error("TTF_SetFontSDF() failed!");
    }
}

bool SDL::TTF::Font::GetSDF() const {
    return TTF_GetFontSDF(m_p_font);
}

void SDL::TTF::Font::SetHorizontalAlignment(TTF_HorizontalAlignment wrap_alignment) {
    TTF_SetFontWrapAlignment(m_p_font, wrap_alignment);
}

void SDL::TTF::Font::GetGlyphMetrics(uint16_t ascii, glm::vec2& min, glm::vec2& max, float& advance) {

    int32_t minx = 0;
    int32_t maxx = 0;
    int32_t miny = 0;
    int32_t maxy = 0;
    int32_t adv = 0;

    if (!TTF_GetGlyphMetrics(m_p_font, ascii, &minx, &maxx, &miny, &maxy, &adv)) {
        throw SDL::Error("TTF_GetGlyphMetrics() failed!");
    }

    min = glm::vec2(static_cast<float>(minx), static_cast<float>(miny));
    max = glm::vec2(static_cast<float>(maxx), static_cast<float>(maxy));
    advance = static_cast<float>(adv);
}

//----------------------------------------------------------------------------------------------------------------------
// TTF Engine

SDL::TTF::TextEngine::TextEngine()
    : m_p_engine(nullptr) {
}

SDL::TTF::TextEngine::TextEngine(GpuDevice& device)
    : m_p_engine(TTF_CreateGPUTextEngine(device.Get())) {
    if (m_p_engine == nullptr) {
        throw SDL::Error("TTF_CreateGPUTextEngine() failed!");
    }
}

SDL::TTF::TextEngine::TextEngine(TextEngine&& other) noexcept
    : m_p_engine(other.m_p_engine) {
    other.m_p_engine = nullptr;
}

SDL::TTF::TextEngine& SDL::TTF::TextEngine::operator=(SDL::TTF::TextEngine&& other) noexcept {

    std::swap(m_p_engine, other.m_p_engine);

    return *this;
}

SDL::TTF::TextEngine::~TextEngine() {
    if (m_p_engine != nullptr) {
        TTF_DestroyGPUTextEngine(m_p_engine);
    }
}

TTF_TextEngine* SDL::TTF::TextEngine::Get() {
    return m_p_engine;
}

//----------------------------------------------------------------------------------------------------------------------
// TTF Text

SDL::TTF::Text::Text()
    : m_p_text(nullptr) {
}

SDL::TTF::Text::Text(TextEngine& engine, Font& font, const std::string& str)
    : m_p_text(TTF_CreateText(engine.Get(), font.Get(), str.c_str(), str.size())) {
    if (m_p_text == nullptr) {
        throw SDL::Error("TTF_CreateText() failed!");
    }
}

SDL::TTF::Text::Text(Text&& other) noexcept
    : m_p_text(other.m_p_text) {
    other.m_p_text = nullptr;
}

SDL::TTF::Text& SDL::TTF::Text::operator=(Text&& other) noexcept {

    std::swap(m_p_text, other.m_p_text);

    return *this;
}

SDL::TTF::Text::~Text() {

    if (m_p_text != nullptr) {
        TTF_DestroyText(m_p_text);
    }
}

void SDL::TTF::Text::SetString(const std::string& str) {

    if(!TTF_SetTextString(m_p_text, str.c_str(), str.length())) {
        throw SDL::Error("TTF_SetTextString() failed!");
    }
}

void SDL::TTF::Text::GetSize(int& width, int& height) {

    if(!TTF_GetTextSize(m_p_text, &width, &height)) {
        throw SDL::Error("TTF_GetTextSize() failed!");
    }
}

void SDL::TTF::Text::GetPosition(int& pos_x, int& pos_y) {

    if(!TTF_GetTextPosition(m_p_text, &pos_x, &pos_y)) {
        throw SDL::Error("TTF_GetPosition() failed!");
    }
}

TTF_GPUAtlasDrawSequence* SDL::TTF::Text::GetGpuDrawData() {

    TTF_GPUAtlasDrawSequence* p_sequence = TTF_GetGPUTextDrawData(m_p_text);
    if (p_sequence == nullptr) {
        throw SDL::Error("TTF_GetGPUTextDrawData() failed!");
    }
    return p_sequence;
}
