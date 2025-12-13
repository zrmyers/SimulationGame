#pragma once

#include "Element.hpp"
#include <cstddef>

namespace UI {

    // Displays only selected child. Size of the element is found by finding the maximal extent of each of the
    // element's children.
    class Switch : public Element {

        public:
            Switch();

            //! determine which child of the element should be displayed.
            void SelectChild(size_t index);

            //! Update the currently displayed graphics.
            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) override;

        private:

            size_t m_selected_child;
    };
}