/**
 * @file HorizontalLayout.hpp
 * @brief Horizontal layout container for arranging child elements left-to-right.
 */

#pragma once

#include "Element.hpp"

// defines the base elements from which a UI can be built.
namespace UI {

    /**
     * @class HorizontalLayout
     * @brief Arranges child elements horizontally and manages sizing/positioning.
     */
    class HorizontalLayout : public Element {

        public:
            HorizontalLayout();

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