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

    //! Represents a button that spawns a drop-down menu when clicked.
    class DropDown : public Element {

        public:

            DropDown();

            DropDown& SetStyle(const std::shared_ptr<DropDownStyle>& p_style);
            DropDown& SelectOption(size_t select);
            DropDown& SetOptions(std::vector<std::string> options);
            DropDown& SetValueChangedCallback(DropDownValueChangeCallback_t callback);

            //! Update the currently displayed graphics.
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