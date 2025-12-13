#pragma once

#include "Element.hpp"

namespace UI {

    class Spacer : public Element {

        public:

            Spacer();

            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) override;
    };
}