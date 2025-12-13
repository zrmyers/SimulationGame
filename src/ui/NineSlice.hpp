#pragma once

#include "Element.hpp"
#include "NineSliceStyle.hpp"
#include "ImageElement.hpp"

namespace UI {

    class NineSlice : public Element {

        public:
            NineSlice() = default;

            //! Set the NineSlice style.
            NineSlice& SetStyle(const std::shared_ptr<NineSliceStyle>& style);

            void CalculateSize(glm::vec2 parent_size) override;

            void CalculatePosition(glm::vec2 parent_size, glm::vec2 parent_position) override;

            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) override;

        private:

            void CalculateSliceSize(glm::vec2 centerSize, float borderWidth);

            std::shared_ptr<NineSliceStyle> m_style;

            // elements used for rendering the nineslice.
            std::vector<std::unique_ptr<UI::ImageElement>> m_borders;
    };
}