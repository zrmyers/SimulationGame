#pragma once

#include "Element.hpp"

// defines the base elements from which a UI can be built.
namespace UI {

    //! An element that constains a list of elements arranged vertically
    class VerticalLayout : public Element {

        public:
            VerticalLayout();

            //! Calculate the size of the element.
            //!
            //! @param[in] parent_size The absolute size of the parent element.
            void CalculateSize(glm::vec2 parent_size) override;

            //! Calculate the position of the element
            //!
            //! @param[in] parent_size The absolute size of the parent element.
            //! @param[in] parent_position The absolute position of the parent element.
            void CalculatePosition(glm::vec2 parent_size, glm::vec2 parent_position) override;

            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) override;
    };
}