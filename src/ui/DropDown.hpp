/**
 * @file DropDown.hpp
 * @brief Declaration for a drop-down control that spawns a selection menu.
 */

#pragma once

#include "DropDownStyle.hpp"
#include "Element.hpp"
#include "ecs/ECS.hpp"
#include "ui/Button.hpp"
#include "ui/VerticalLayout.hpp"
#include <functional>
#include <memory>
#include <string>

namespace UI {

    using DropDownValueChangeCallback_t = std::function<void(size_t)>;

    /**
     * @class DropDown
     * @brief Button-like element that opens a list of selectable options on click.
     */
    class DropDown : public Element {

        public:

            /** @brief Default constructor. */
            DropDown();

            /**
             * @brief Set the style used by this drop-down.
             * @param p_style Shared pointer to a DropDownStyle.
             * @return Reference to this DropDown.
             */
            DropDown& SetStyle(const std::shared_ptr<DropDownStyle>& p_style);

            /**
             * @brief Select an option by index.
             * @param select Index to select.
             * @return Reference to this DropDown.
             */
            DropDown& SelectOption(size_t select);

            /**
             * @brief Set the available options for the drop-down.
             * @param options Vector of option strings.
             * @return Reference to this DropDown.
             */
            DropDown& SetOptions(std::vector<std::string> options);

            /**
             * @brief Set callback invoked when selection changes.
             * @param callback Function called with new selected index.
             * @return Reference to this DropDown.
             */
            DropDown& SetValueChangedCallback(DropDownValueChangeCallback_t callback);

            /**
             * @brief Update the currently displayed graphics for the drop-down.
             * @param registry ECS registry used for rendering components.
             * @param screenSize Screen resolution in pixels.
             * @param depth Draw order depth.
             */
            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) override;

        private:

            //! Spawn drop-down menu
            void SpawnDropDown(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth);

            //! De-spawn drop-down menu
            void DespawnDropDown(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth);

            //! The style to use for spawned buttons.
            std::shared_ptr<DropDownStyle> m_p_style;

            //! The list of options
            std::vector<std::string> m_options;

            //! The currently selected position.
            size_t m_selected;

            //! Pointer to the button that shows current selection, and spawns the drop down menu on click.
            Button* m_p_selection_button;

            //! Entity used for creating drop down menu on new canvas.
            ECS::Entity m_dropdown_entity;

            //! The callback function when drop down value changes.
            DropDownValueChangeCallback_t m_value_change_callback;

            //! Whether selection is in progress.
            bool m_selection_in_progress;
    };

}