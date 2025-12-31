#pragma once

#include "NineSliceStyle.hpp"
#include "graphics/Font.hpp"
#include "graphics/Texture2D.hpp"

#include <cstdint>
#include <memory>
#include <string>

namespace UI {

    enum class SliderState : uint8_t {
        UNKNOWN = 0,
        ENABLED,
        FOCUSED
    };

    std::string ToString(SliderState state);

    class SliderStyle {

        public:

            SliderStyle() = default;

            void SetFont(const std::shared_ptr<Graphics::Font>& p_font);
            const std::shared_ptr<Graphics::Font>& GetFont() const;

            void SetTrackFilledStyle(SliderState state, const std::shared_ptr<NineSliceStyle>& p_style);
            const std::shared_ptr<NineSliceStyle>& GetTrackFilledStyle(SliderState state) const;

            void SetTrackUnfilledStyle(SliderState state, const std::shared_ptr<NineSliceStyle>& p_style);
            const std::shared_ptr<NineSliceStyle>& GetTrackUnfilledStyle(SliderState state) const;

            void SetKnobImage(SliderState state, const std::shared_ptr<Graphics::Texture2D>& p_image);
            const std::shared_ptr<Graphics::Texture2D>& GetKnobImage(SliderState state) const;

        private:

            //! style for checkboxes used to implement radio selection
            std::unordered_map<SliderState, std::shared_ptr<NineSliceStyle>> m_track_filled_style;
            std::unordered_map<SliderState, std::shared_ptr<NineSliceStyle>> m_track_unfilled_style;
            std::unordered_map<SliderState, std::shared_ptr<Graphics::Texture2D>> m_knob_style;
            std::shared_ptr<Graphics::Font> m_p_font;
    };
}