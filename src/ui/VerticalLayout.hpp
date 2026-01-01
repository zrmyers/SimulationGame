/**
 * @file VerticalLayout.hpp
 * @brief Vertical layout container for arranging child elements top-to-bottom.
 */

#pragma once

#include "Element.hpp"

// defines the base elements from which a UI can be built.
namespace UI {

    /**
     * @class VerticalLayout
     * @brief Arranges child elements vertically and manages sizing/positioning.
     */
    class VerticalLayout : public Element {

        public:
            VerticalLayout();

            /**
             * @brief Calculate the size of the element.
             * @param parent_size The absolute size of the parent element.
             */
            void CalculateSize(glm::vec2 parent_size) override;

            /**
             * @brief Calculate the position of the element
             * @param parent_size The absolute size of the parent element.
             * @param parent_position The absolute position of the parent element.
             */
            void CalculatePosition(glm::vec2 parent_size, glm::vec2 parent_position) override;

            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) override;
    };
}