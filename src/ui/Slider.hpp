/**
 * @file Slider.hpp
 * @brief Slider widget declaration for selecting a value from a range of options.
 */

#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include "Element.hpp"
#include "NineSlice.hpp"
#include "SliderStyle.hpp"
#include "TextElement.hpp"
#include "ui/ImageElement.hpp"

namespace UI {

    using SliderValueChangeCallback = std::function<void(size_t)>;

    /**
     * @class Slider
     * @brief UI element representing a slider control with knob and value label.
     */
    class Slider : public UI::Element {

        public:

            /** @brief Constructor. */
            Slider();

            /**
             * @brief Set the style used for this slider.
             * @param p_style Shared pointer to a SliderStyle.
             * @return Reference to this Slider.
             */
            Slider& SetStyle(const std::shared_ptr<SliderStyle>& p_style);

            /**
             * @brief Select an option by index.
             * @param select Index to select (0-based).
             * @return Reference to this Slider.
             */
            Slider& SelectOption(size_t select);

            /**
             * @brief Set the available option strings displayed by the slider.
             * @param options Vector of option strings.
             * @return Reference to this Slider.
             */
            Slider& SetOptions(std::vector<std::string> options);

            /**
             * @brief Set a callback invoked when the slider value changes.
             * @param callback Function taking the new selected index.
             * @return Reference to this Slider.
             */
            Slider& SetValueChangedCallback(SliderValueChangeCallback callback);

            /**
             * @brief Set the maximum number of characters for the value label.
             * @param max_chars Maximum character count.
             * @return Reference to this Slider.
             */
            Slider& SetMaxLabelCharacters(uint8_t max_chars);

            /**
             * @brief Set max fixed width for the slide track.
             * @param width Maximum slider track width in pixels.
             * @return Reference to this Slider.
             */
            Slider& SetMaxSliderWidth(float width);
            /**
             * @brief Set the visual state of the slider (e.g., focused/enabled).
             * @param state SliderState to apply.
             * @return Reference to this Slider.
             */
            Slider& SetSliderState(SliderState state);

            /**
             * @brief Update the currently displayed graphics for the slider.
             * @param registry ECS registry used for rendering components.
             * @param screenSize Screen resolution in pixels.
             * @param depth Draw order depth.
             */
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

            //! Maximum characters for the value label.
            uint8_t m_max_label_characters;

            //! Maximum width for the slider track.
            float m_max_slider_width;

            //! Whether the slider is currently tracking the mouse.
            bool m_track_mouse;
    };
}