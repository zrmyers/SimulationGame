/**
 * @file CheckBox.hpp
 * @brief Declaration for checkbox UI widget and its callbacks.
 */

#pragma once

#include "CheckBoxStyle.hpp"
#include "Element.hpp"
#include "ui/ImageElement.hpp"
#include <functional>
#include <memory>

namespace UI {

    /**
     * @brief Callback when the check box state changes. The parameter is true when checked.
     */
    using CheckBoxStateCallback_t = std::function<void(bool)>;

    /**
     * @class CheckBox
     * @brief Toggleable checkbox element with on/off states and focus/activation states.
     */
    class CheckBox : public Element {

        public:
            /** @brief Default constructor. */
            CheckBox();

            /**
             * @brief Set the checkbox style resource.
             * @param p_style Shared pointer to a CheckBoxStyle.
             * @return Reference to this CheckBox.
             */
            CheckBox& SetStyle(std::shared_ptr<CheckBoxStyle> p_style);

            /**
             * @brief Set callback invoked when checkbox changes state.
             * @param callback Function called with new checked state (true = on).
             * @return Reference to this CheckBox.
             */
            CheckBox& SetCheckBoxStateCallback(CheckBoxStateCallback_t callback);

            /**
             * @brief Set the visual/interaction state for the checkbox.
             * @param state CheckBoxState to set.
             * @return Reference to this CheckBox.
             */
            CheckBox& SetCheckBoxState(CheckBoxState state);

            /**
             * @brief Disable toggling off when the checkbox is clicked while on.
             * @param disabled True to disable toggling off.
             * @return Reference to this CheckBox.
             */
            CheckBox& SetToggleOffDisabled(bool disabled);

            /**
             * @brief Update the currently displayed graphics for the checkbox.
             * @param registry ECS registry used for rendering components.
             * @param screenSize Screen resolution in pixels.
             * @param depth Draw order depth.
             */
            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) override;

        private:

            std::shared_ptr<CheckBoxStyle> m_p_style;
            CheckBoxStateCallback_t m_state_callback;
            ImageElement* m_p_image_element;
            CheckBoxState m_current_state;
            bool m_disable_off_toggle; // whether clicking button when on will toggle the state.
    };
}