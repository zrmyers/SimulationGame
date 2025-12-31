#pragma once
#include <cstddef>
#include <functional>
#include "Element.hpp"
#include "NineSlice.hpp"
#include "SliderStyle.hpp"
#include "TextElement.hpp"
#include "ui/ImageElement.hpp"

namespace UI {

    using SliderValueChangeCallback = std::function<void(size_t)>;

    class Slider : public UI::Element {

        public:

            //! Constructor.
            Slider();

            //! Set the style of the slider.
            Slider& SetStyle(const std::shared_ptr<SliderStyle>& p_style);

            //! Set the index of the selected option.
            Slider& SelectOption(size_t select);

            //! List of strings to display for selected option.
            Slider& SetOptions(std::vector<std::string> options);

            //! User callback when slider value changes
            Slider& SetValueChangedCallback(SliderValueChangeCallback callback);

            //! Used to switch between focused and enabled colors
            Slider& SetSliderState(SliderState state);

            //! Update the currently displayed graphics.
            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) override;

        private:

            //! The style to use for spawned buttons.
            std::shared_ptr<SliderStyle> m_p_style;

            //! The list of options
            std::vector<std::string> m_options;

            //! The current state of the slider.
            SliderState m_state;

            //! The currently selected position.
            size_t m_selected;

            //! Pointers to selection checkboxes
            UI::NineSlice* m_p_filled;
            UI::NineSlice* m_p_unfilled;
            UI::ImageElement* m_p_knob;
            UI::TextElement* m_p_value_label;

            //! The callback function when drop down value changes.
            SliderValueChangeCallback m_value_change_callback;

            //! Whether the slider is currently tracking the mouse.
            bool m_track_mouse;

    };
}