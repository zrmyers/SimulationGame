/**
 * @file Radio.hpp
 * @brief Radio button group declaration for selecting one option among many.
 */

#pragma once
#include <functional>
#include "Element.hpp"
#include "RadioStyle.hpp"
#include "ui/CheckBox.hpp"

namespace UI {

    using RadioValueChangeCallback_t = std::function<void(size_t)>;

    /**
     * @class Radio
     * @brief A group of mutually-exclusive options implemented using checkboxes.
     */
    class Radio : public UI::Element {

        public:
            /** @brief Default constructor. */
            Radio();

            /**
             * @brief Set the style for this radio group.
             * @param p_style Shared pointer to a RadioStyle.
             * @return Reference to this Radio.
             */
            Radio& SetStyle(const std::shared_ptr<RadioStyle>& p_style);

            /**
             * @brief Select an option by index.
             * @param select Index to select.
             * @return Reference to this Radio.
             */
            Radio& SelectOption(size_t select);

            /**
             * @brief Set the available options for the radio group.
             * @param options Vector of option strings.
             * @return Reference to this Radio.
             */
            Radio& SetOptions(std::vector<std::string> options);

            /**
             * @brief Set a callback invoked when the selected option changes.
             * @param callback Function called with the new selected index.
             * @return Reference to this Radio.
             */
            Radio& SetValueChangedCallback(RadioValueChangeCallback_t callback);

            /**
             * @brief Update graphics for the radio group and its children.
             * @param registry ECS registry used for rendering components.
             * @param screenSize Screen resolution in pixels.
             * @param depth Draw order depth.
             */
            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) override;

        private:

            //! The style to use for spawned buttons.
            std::shared_ptr<RadioStyle> m_p_style;

            //! The list of options
            std::vector<std::string> m_options;

            //! The currently selected position.
            size_t m_selected;

            //! Pointers to selection checkboxes
            std::vector<UI::CheckBox*> m_p_buttons;
            UI::CheckBox* m_p_selected;

            //! The callback function when drop down value changes.
            RadioValueChangeCallback_t m_value_change_callback;

    };
}