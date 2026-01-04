#include "Style.hpp"

#include <SDL3/SDL_gpu.h>
#include <fstream>
#include <memory>
#include <string>
#include "CheckBoxStyle.hpp"
#include "DropDownStyle.hpp"
#include "NineSliceStyle.hpp"
#include "SliderStyle.hpp"
#include "core/AssetLoader.hpp"
#include "core/Engine.hpp"
#include "graphics/Texture2D.hpp"
#include "nlohmann/json.hpp"
#include "sdl/SDL.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/TextSystem.hpp"
#include "ui/RadioStyle.hpp"
#include "ui/TextInputBoxStyle.hpp"

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
            std::shared_ptr<NineSliceStyle> nineSliceStyle = nullptr;

            if (nineSliceData.contains("image")) {

                // need to slice up image into parts
                std::string image_id = nineSliceData["image"];
                int borderWidth = nineSliceData["border-width-px"];

                nineSliceStyle = NineSliceStyle::Load(engine, image_id, borderWidth);

            } else {

                // button has been pre-sliced.
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

                nineSliceStyle = NineSliceStyle::Load(engine, imageFiles);
            }

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

    if (styleData.contains("dropdown")) {
        for (auto& dropdownData : styleData["dropdown"]) {

            std::string dropdown_id = dropdownData["id"];

            std::shared_ptr<DropDownStyle> p_style = std::make_shared<DropDownStyle>();

            p_style->SetSelectionButtonStyle(style.GetButtonStyle(dropdownData["selection-button-id"]));
            p_style->SetOptionsButtonStyle(style.GetButtonStyle(dropdownData["options-button-id"]));

            style.SetDropDownStyle(dropdown_id, std::move(p_style));
        }
    }

    if (styleData.contains("radio")) {
        for (auto& radioData : styleData["radio"]) {

            std::string radio_id = radioData["id"];

            std::shared_ptr<RadioStyle> p_radioStyle = std::make_shared<RadioStyle>();

            p_radioStyle->SetFont(style.GetFont(radioData["font-id"]));
            p_radioStyle->SetCheckboxStyle(style.GetCheckBoxStyle(radioData["checkbox-id"]));

            style.SetRadioStyle(radio_id, std::move(p_radioStyle));
        }
    }

    if (styleData.contains("slider")) {

        for (auto& sliderData : styleData["slider"]) {

            std::string slider_id = sliderData["id"];
            std::shared_ptr<SliderStyle> p_sliderStyle = std::make_shared<SliderStyle>();

            p_sliderStyle->SetFont(style.GetFont(sliderData["font-id"]));

            // track filled style
            auto& trackFilledData = sliderData["track-filled"];
            p_sliderStyle->SetTrackFilledStyle(SliderState::ENABLED, style.GetNineSliceStyle(trackFilledData["enabled-nineslice-id"]));
            p_sliderStyle->SetTrackFilledStyle(SliderState::FOCUSED, style.GetNineSliceStyle(trackFilledData["focused-nineslice-id"]));

            // track unfilled style
            auto& trackUnfilledData = sliderData["track-unfilled"];
            p_sliderStyle->SetTrackUnfilledStyle(SliderState::ENABLED, style.GetNineSliceStyle(trackUnfilledData["enabled-nineslice-id"]));
            p_sliderStyle->SetTrackUnfilledStyle(SliderState::FOCUSED, style.GetNineSliceStyle(trackUnfilledData["focused-nineslice-id"]));

            // knob style
            auto& knobData = sliderData["knob"];
            p_sliderStyle->SetKnobImage(SliderState::ENABLED, LoadImage(engine, knobData["enabled-image-id"]));
            p_sliderStyle->SetKnobImage(SliderState::FOCUSED, LoadImage(engine, knobData["focused-image-id"]));

            style.SetSliderStyle(slider_id, std::move(p_sliderStyle));
        }
    }

    if (styleData.contains("text-input-box")) {

        for (auto& boxData : styleData["text-input-box"]) {

            std::string box_id = boxData["id"];
            std::shared_ptr<TextInputBoxStyle> p_style = std::make_shared<TextInputBoxStyle>();

            p_style->SetTextFont(style.GetFont(boxData["font-id"]));
            p_style->SetBoxStyle(TextInputState::ENABLED, style.GetNineSliceStyle(boxData["enabled-nineslice-id"]));
            p_style->SetBoxStyle(TextInputState::FOCUSED, style.GetNineSliceStyle(boxData["focused-nineslice-id"]));
            p_style->SetDefaultTextColor(ParseColor(boxData["default-text-color"]));
            p_style->SetNormalTextColor(ParseColor(boxData["text-color"]));
            p_style->SetCaretStyle(style.GetNineSliceStyle(boxData["caret-nineslice-id"]));

            style.SetTextInputBoxStyle(box_id, std::move(p_style));
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
    m_nine_slice_styles[style_id] = std::move(style);
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
    m_checkbox_style[checkbox_id] = std::move(style);
}

std::shared_ptr<UI::CheckBoxStyle>& UI::Style::GetCheckBoxStyle(const std::string& checkbox_id) {
    auto checkboxIter = m_checkbox_style.find(checkbox_id);
    if (checkboxIter == m_checkbox_style.end()) {
        throw Core::EngineException("GetCheckBoxStyle() failed to find style " + checkbox_id);
    }
    return checkboxIter->second;
}

void UI::Style::SetDropDownStyle(const std::string& dropdown_id, std::shared_ptr<DropDownStyle>&& style) {
    m_dropdown_style[dropdown_id] = std::move(style);
}

std::shared_ptr<UI::DropDownStyle>& UI::Style::GetDropDownStyle(const std::string& dropdown_id) {
    auto dropdownIter = m_dropdown_style.find(dropdown_id);
    if (dropdownIter == m_dropdown_style.end()) {
        throw Core::EngineException("GetDropDownStyle() failed to find style " + dropdown_id);
    }
    return dropdownIter->second;
}

void UI::Style::SetRadioStyle(const std::string& radio_id, std::shared_ptr<RadioStyle>&& style) {
    m_radio_styles[radio_id] = std::move(style);
}

std::shared_ptr<UI::RadioStyle>& UI::Style::GetRadioStyle(const std::string& radio_id) {
    auto radioIter = m_radio_styles.find(radio_id);
    if (radioIter == m_radio_styles.end()) {
        throw Core::EngineException("GetRadioStyle() failed to find style " + radio_id);
    }

    return radioIter->second;
}

void UI::Style::SetSliderStyle(const std::string& slider_id, std::shared_ptr<SliderStyle>&& style) {
    m_slider_styles[slider_id] = std::move(style);
}

std::shared_ptr<UI::SliderStyle>& UI::Style::GetSliderStyle(const std::string& slider_id) {
    auto styleIter = m_slider_styles.find(slider_id);
    if (styleIter == m_slider_styles.end()) {
        throw Core::EngineException("GetSliderStyle() failed to find style " + slider_id);
    }
    return styleIter->second;
}

void UI::Style::SetTextInputBoxStyle(const std::string& box_id, std::shared_ptr<TextInputBoxStyle> p_style) {
    m_text_input_box_styles[box_id] = std::move(p_style);
}

std::shared_ptr<UI::TextInputBoxStyle>& UI::Style::GetTextInputBoxStyle(const std::string& box_id) {
    auto iter = m_text_input_box_styles.find(box_id);
    if (iter == m_text_input_box_styles.end()) {
        throw Core::EngineException("GetTextInputBoxStyle() failed to find style " + box_id);
    }
    return iter->second;
}

/**
 * @brief Parse a horizontal text alignment from a string.
 * @param asString Alignment as a string ("center", "left", "right").
 * @return Corresponding `TTF_HorizontalAlignment` enum value.
 */
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

/**
 * @brief Parse a JSON color object into an RGBA vector.
 * @param colorData JSON object containing numeric fields `r`, `g`, `b`, and `a`.
 * @return glm::vec4 where each component is in the order (r, g, b, a).
 */
glm::vec4 UI::Style::ParseColor(nlohmann::json& colorData) {

    glm::vec4 color;
    color.r = colorData["r"];
    color.g = colorData["g"];
    color.b = colorData["b"];
    color.a = colorData["a"];
    return color;
}

/**
 * @brief Load an image file into a `Graphics::Texture2D`.
 * @param engine Engine used to access asset loader and render system.
 * @param name Relative filename of the image to load.
 * @return Shared pointer to the populated `Graphics::Texture2D`.
 */
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