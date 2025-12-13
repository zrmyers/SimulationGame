#include "Style.hpp"

#include <SDL3/SDL_gpu.h>
#include <fstream>
#include <memory>
#include "CheckBoxStyle.hpp"
#include "core/AssetLoader.hpp"
#include "core/Engine.hpp"
#include "graphics/Texture2D.hpp"
#include "nlohmann/json.hpp"
#include "sdl/SDL.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/TextSystem.hpp"

//----------------------------------------------------------------------------------------------------------------------
// Style


UI::Style UI::Style::Load(Core::Engine& engine, const std::string& filename) {

    Core::AssetLoader& assetLoader = engine.GetAssetLoader();
    Systems::TextSystem& textSystem = engine.GetEcsRegistry().GetSystem<Systems::TextSystem>();
    UI::Style style;

    std::ifstream filestream(assetLoader.GetUiDir() + "/" + filename);

    nlohmann::json styleData = nlohmann::json::parse(filestream);

    if (styleData.contains("font")) {

        for (auto& fontData : styleData["font"]) {

            std::string fontID = fontData["id"];
            float fontSize = fontData["pt"];
            bool useSDF = fontData["sdf"];
            std::string name = fontData["filename"];
            TTF_HorizontalAlignment alignment = ParseAlignment(fontData["alignment"]);

            std::shared_ptr<Graphics::Font> pFont =
                std::make_shared<Graphics::Font>(assetLoader, textSystem, name, fontSize, useSDF, alignment);

            style.SetFont(fontID, pFont);
        }
    }

    if (styleData.contains("nine-slice")) {

        for (auto& nineSliceData : styleData["nine-slice"]) {

            std::string ninesliceId = nineSliceData["id"];
            std::vector<std::string> imageFiles;
            imageFiles.reserve(NineSliceStyle::SLICE_COUNT);
            imageFiles.push_back(nineSliceData["top-left"]);
            imageFiles.push_back(nineSliceData["top-right"]);
            imageFiles.push_back(nineSliceData["bottom-left"]);
            imageFiles.push_back(nineSliceData["bottom-right"]);
            imageFiles.push_back(nineSliceData["top"]);
            imageFiles.push_back(nineSliceData["left"]);
            imageFiles.push_back(nineSliceData["right"]);
            imageFiles.push_back(nineSliceData["bottom"]);
            imageFiles.push_back(nineSliceData["center"]);

            std::shared_ptr<NineSliceStyle> nineSliceStyle = NineSliceStyle::Load(engine, imageFiles);

            style.SetNineSliceStyle(ninesliceId, std::move(nineSliceStyle));
        }
    }

    if (styleData.contains("button")) {

        for (auto& buttonData : styleData["button"]) {

            std::string buttonId = buttonData["id"];
            std::shared_ptr<ButtonStyle> p_buttonStyle = std::make_shared<ButtonStyle>();

            p_buttonStyle->SetFont(style.GetFont(buttonData["font-id"]));

            auto& disabledData = buttonData["disabled"];
            p_buttonStyle->SetNineSliceStyle(ButtonState::DISABLED, style.GetNineSliceStyle(disabledData["nine-slice-id"]));
            p_buttonStyle->SetTextColor(ButtonState::DISABLED, ParseColor(disabledData["text-color"]));

            auto& enabledData = buttonData["enabled"];
            p_buttonStyle->SetNineSliceStyle(ButtonState::ENABLED, style.GetNineSliceStyle(enabledData["nine-slice-id"]));
            p_buttonStyle->SetTextColor(ButtonState::ENABLED, ParseColor(enabledData["text-color"]));

            auto& focusData = buttonData["focused"];
            p_buttonStyle->SetNineSliceStyle(ButtonState::FOCUSED, style.GetNineSliceStyle(focusData["nine-slice-id"]));
            p_buttonStyle->SetTextColor(ButtonState::FOCUSED, ParseColor(focusData["text-color"]));

            auto& activatedData = buttonData["activated"];
            p_buttonStyle->SetNineSliceStyle(ButtonState::ACTIVATED, style.GetNineSliceStyle(activatedData["nine-slice-id"]));
            p_buttonStyle->SetTextColor(ButtonState::ACTIVATED, ParseColor(activatedData["text-color"]));

            auto& selectedData = buttonData["selected"];
            p_buttonStyle->SetNineSliceStyle(ButtonState::SELECTED, style.GetNineSliceStyle(selectedData["nine-slice-id"]));
            p_buttonStyle->SetTextColor(ButtonState::SELECTED, ParseColor(selectedData["text-color"]));

            style.SetButtonStyle(buttonId, std::move(p_buttonStyle));
        }
    }

    if (styleData.contains("checkbox")) {
        for (auto& checkboxData : styleData["checkbox"]) {

            std::string checkbox_id = checkboxData["id"];

            std::shared_ptr<CheckBoxStyle> p_checkBoxStyle = std::make_shared<CheckBoxStyle>();

            p_checkBoxStyle->SetImage(CheckBoxState::OFF, LoadImage(engine, checkboxData["off"]));
            p_checkBoxStyle->SetImage(CheckBoxState::ON, LoadImage(engine, checkboxData["on"]));
            p_checkBoxStyle->SetImage(CheckBoxState::ON_FOCUSED, LoadImage(engine, checkboxData["on-focused"]));
            p_checkBoxStyle->SetImage(CheckBoxState::OFF_FOCUSED, LoadImage(engine, checkboxData["off-focused"]));
            p_checkBoxStyle->SetImage(CheckBoxState::ON_ACTIVATED, LoadImage(engine, checkboxData["on-activated"]));
            p_checkBoxStyle->SetImage(CheckBoxState::OFF_ACTIVATED, LoadImage(engine, checkboxData["off-activated"]));

            style.SetCheckBoxStyle(checkbox_id, std::move(p_checkBoxStyle));
        }
    }
    return style;
}

void UI::Style::SetFont(const std::string& font_id, std::shared_ptr<Graphics::Font> p_font) {
    m_fonts[font_id] = std::move(p_font);
}

std::shared_ptr<Graphics::Font>& UI::Style::GetFont(const std::string& font_id) {
    auto fontIter = m_fonts.find(font_id);
    if (fontIter == m_fonts.end()) {
        throw Core::EngineException("GetFont() failed to find font " + font_id);
    }
    return fontIter->second;
}

void UI::Style::SetNineSliceStyle(const std::string& style_id, std::shared_ptr<NineSliceStyle>&& style) {
    m_nine_slice_styles[style_id] = style;
}

std::shared_ptr<UI::NineSliceStyle>& UI::Style::GetNineSliceStyle(const std::string& nineslice_id) {
    auto styleIter = m_nine_slice_styles.find(nineslice_id);
    if (styleIter == m_nine_slice_styles.end()) {
        throw Core::EngineException("GetNineSliceStyle() failed to find style " + nineslice_id);
    }
    return styleIter->second;
}

void UI::Style::SetButtonStyle(const std::string& button_id, std::shared_ptr<ButtonStyle>&& style) {
    m_button_style[button_id] = std::move(style);
}

std::shared_ptr<UI::ButtonStyle>& UI::Style::GetButtonStyle(const std::string& button_id) {
    auto buttonIter = m_button_style.find(button_id);
    if (buttonIter == m_button_style.end()) {
        throw Core::EngineException("GetButtonStyle() failed to find style " + button_id);
    }
    return buttonIter->second;
}

void UI::Style::SetCheckBoxStyle(const std::string& checkbox_id, std::shared_ptr<CheckBoxStyle>&& style) {
    m_checkbox_style[checkbox_id] = style;
}

std::shared_ptr<UI::CheckBoxStyle>& UI::Style::GetCheckBoxStyle(const std::string& checkbox_id) {
    auto checkboxIter = m_checkbox_style.find(checkbox_id);
    if (checkboxIter == m_checkbox_style.end()) {
        throw Core::EngineException("GetCheckBoxStyle() failed to find style " + checkbox_id);
    }
    return checkboxIter->second;
}

TTF_HorizontalAlignment UI::Style::ParseAlignment(const std::string& asString) {
    TTF_HorizontalAlignment alignment = TTF_HORIZONTAL_ALIGN_INVALID;

    if (asString == "center") {
        alignment = TTF_HORIZONTAL_ALIGN_CENTER;
    }
    else if (asString == "left") {
        alignment = TTF_HORIZONTAL_ALIGN_LEFT;
    }
    else if (asString == "right") {
        alignment = TTF_HORIZONTAL_ALIGN_RIGHT;
    }

    return alignment;
}

glm::vec4 UI::Style::ParseColor(nlohmann::json& colorData) {

    glm::vec4 color;
    color.r = colorData["r"];
    color.g = colorData["g"];
    color.b = colorData["b"];
    color.a = colorData["a"];
    return color;
}

std::shared_ptr<Graphics::Texture2D> UI::Style::LoadImage(Core::Engine& engine, const std::string& name) {

    Systems::RenderSystem& renderSystem = engine.GetEcsRegistry().GetSystem<Systems::RenderSystem>();
    Core::AssetLoader& assetLoader = engine.GetAssetLoader();

    SDL::Image image(assetLoader.GetImageDir() + "/" + name);

    SDL_GPUSamplerCreateInfo samplerInfo = {};
    samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.enable_anisotropy = true;
    samplerInfo.max_anisotropy = 16;

    std::shared_ptr<SDL::GpuSampler> pSampler
         = std::make_shared<SDL::GpuSampler>(renderSystem.CreateSampler(samplerInfo));

    std::shared_ptr<Graphics::Texture2D> pTexture
        = std::make_shared<Graphics::Texture2D>(engine,std::move(pSampler),image.GetWidth(), image.GetHeight(), false);

    pTexture->LoadImageData(image);

    return pTexture;
}