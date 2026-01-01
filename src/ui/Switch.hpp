/**
 * @file Switch.hpp
 * @brief `Switch` element shows only one child at a time (like a view switcher).
 */

#pragma once

#include "Element.hpp"
#include <cstddef>

namespace UI {

    /**
     * @class Switch
     * @brief Displays only the selected child element; useful for view switching.
     */
    class Switch : public Element {

        public:
            Switch();

            /**
             * @brief Determine which child of the element should be displayed.
             * @param index Index of child to select.
             */
            void SelectChild(size_t index);

            /**
             * @brief Update the currently displayed graphics for the selected child.
             */
            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) override;

        private:

            size_t m_selected_child;
    };
}