/**
 * @file Spacer.hpp
 * @brief Simple empty element used to create spacing in layouts.
 */

#pragma once

#include "Element.hpp"

namespace UI {

    /**
     * @class Spacer
     * @brief Invisible element used purely for layout spacing.
     */
    class Spacer : public Element {

        public:

            Spacer();

            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) override;
    };
}