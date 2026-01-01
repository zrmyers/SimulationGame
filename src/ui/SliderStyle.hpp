/**
 * @file SliderStyle.hpp
 * @brief Slider styling declarations for UI widgets.
 */

#pragma once

#include "NineSliceStyle.hpp"
#include "graphics/Font.hpp"
#include "graphics/Texture2D.hpp"

#include <cstdint>
#include <memory>
#include <string>

namespace UI {

    /**
     * @enum SliderState
     * @brief Visual state for a slider element.
     */
    enum class SliderState : uint8_t {
        UNKNOWN = 0,
        ENABLED,
        FOCUSED
    };

    /**
     * @brief Convert a SliderState to a human-readable string.
     * @param state The slider state to convert.
     * @return A string representation of @p state.
     */
    std::string ToString(SliderState state);

    /**
     * @class SliderStyle
     * @brief Encapsulates visual styling for slider widgets (fonts, track, knob).
     */
    class SliderStyle {

        public:

            SliderStyle() = default;

            /**
             * @brief Set the font used for slider labels.
             * @param p_font Shared pointer to a font resource.
             */
            void SetFont(const std::shared_ptr<Graphics::Font>& p_font);
            /**
             * @brief Get the configured font.
             * @return Shared pointer to the font resource.
             */
            const std::shared_ptr<Graphics::Font>& GetFont() const;

            /**
             * @brief Set the nine-slice style used for the filled portion of the slider track for a given state.
             * @param state SliderState to apply.
             * @param p_style Shared pointer to a NineSliceStyle.
             */
            void SetTrackFilledStyle(SliderState state, const std::shared_ptr<NineSliceStyle>& p_style);
            /**
             * @brief Get the nine-slice style used for the filled portion of the slider track.
             * @param state SliderState to query.
             * @return Shared pointer to the NineSliceStyle for the filled track.
             */
            const std::shared_ptr<NineSliceStyle>& GetTrackFilledStyle(SliderState state) const;

            /**
             * @brief Set the nine-slice style used for the unfilled portion of the slider track for a given state.
             * @param state SliderState to apply.
             * @param p_style Shared pointer to a NineSliceStyle.
             */
            void SetTrackUnfilledStyle(SliderState state, const std::shared_ptr<NineSliceStyle>& p_style);
            /**
             * @brief Get the nine-slice style used for the unfilled portion of the slider track.
             * @param state SliderState to query.
             * @return Shared pointer to the NineSliceStyle for the unfilled track.
             */
            const std::shared_ptr<NineSliceStyle>& GetTrackUnfilledStyle(SliderState state) const;

            /**
             * @brief Set the knob image for a given slider state.
             * @param state SliderState to apply.
             * @param p_image Shared pointer to a Texture2D.
             */
            void SetKnobImage(SliderState state, const std::shared_ptr<Graphics::Texture2D>& p_image);
            /**
             * @brief Get the knob image for a given slider state.
             * @param state SliderState to query.
             * @return Shared pointer to the Texture2D used for the knob.
             */
            const std::shared_ptr<Graphics::Texture2D>& GetKnobImage(SliderState state) const;

            /**
             * @brief Apply a fixed size for the slider widget based on style.
             */
            void SetSliderFixedSize();
            
        private:

            //! style for checkboxes used to implement radio selection
            std::unordered_map<SliderState, std::shared_ptr<NineSliceStyle>> m_track_filled_style;
            std::unordered_map<SliderState, std::shared_ptr<NineSliceStyle>> m_track_unfilled_style;
            std::unordered_map<SliderState, std::shared_ptr<Graphics::Texture2D>> m_knob_style;
            std::shared_ptr<Graphics::Font> m_p_font;
    };
}