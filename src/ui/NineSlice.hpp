/**
 * @file NineSlice.hpp
 * @brief Nine-slice UI element declaration for scalable framed backgrounds.
 */

#pragma once

#include "Element.hpp"
#include "NineSliceStyle.hpp"
#include "ImageElement.hpp"

namespace UI {

    /**
     * @class NineSlice
     * @brief A nine-slice element composes 9 image regions to create scalable framed UI backgrounds.
     */
    class NineSlice : public Element {

        public:
            NineSlice() = default;

            /**
             * @brief Set the NineSlice style (textures and border width).
             * @param style Shared pointer to a NineSliceStyle.
             * @return Reference to this NineSlice.
             */
            NineSlice& SetStyle(const std::shared_ptr<NineSliceStyle>& style);

            /**
             * @brief Calculate element size using nine-slice rules.
             * @param parent_size The absolute size of the parent element.
             */
            void CalculateSize(glm::vec2 parent_size) override;

            /**
             * @brief Calculate child and slice positions.
             * @param parent_size The absolute size of the parent element.
             * @param parent_position The absolute position of the parent element.
             */
            void CalculatePosition(glm::vec2 parent_size, glm::vec2 parent_position) override;

            /**
             * @brief Update graphics for the nine-slice and its children.
             * @param registry ECS registry used for rendering components.
             * @param screenSize Screen resolution in pixels.
             * @param depth Draw order depth.
             */
            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) override;

            /** @brief Clear rendering entities for the nine-slice and its children. */
            void ClearGraphics() override;

            /**
             * @brief Set the visibility of the nine-slice element.
             * @param isVisible True to make visible, false to hide.
             */
            void SetVisible(bool isVisible);

            /**
             * @brief Get the visibility of the nine-slice element.
             * @return True if visible, false otherwise.
             */
            bool GetVisible() const;

        private:

            void CalculateSliceSize(glm::vec2 centerSize, float borderWidth);

            std::shared_ptr<NineSliceStyle> m_style;

            // elements used for rendering the nineslice.
            std::vector<std::unique_ptr<UI::ImageElement>> m_borders;

            bool m_is_visible {true};
    };
}