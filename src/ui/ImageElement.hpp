/**
 * @file ImageElement.hpp
 * @brief Declaration for an image UI element (renders a texture).
 */

#pragma once

#include "Element.hpp"
#include "ecs/ECS.hpp"
#include "graphics/Texture2D.hpp"

namespace UI {

    /**
     * @class ImageElement
     * @brief UI element that displays a single texture as a quad sprite.
     */
    class ImageElement : public Element {

        public:

            /** @brief Default constructor. */
            ImageElement();

            /**
             * @brief Set the texture to display.
             * @param p_texture Shared pointer to the texture resource.
             * @return Reference to this ImageElement.
             */
            ImageElement& SetTexture(std::shared_ptr<Graphics::Texture2D> p_texture);

            /**
             * @brief Update graphics for the image element.
             * @param registry ECS registry used for rendering components.
             * @param screenSize Screen resolution in pixels.
             * @param depth Draw order depth.
             */
            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, Depth_t depth) override;

            /** @brief Clear any created rendering entities for this element. */
            void ClearGraphics() override;
        private:

            std::shared_ptr<Graphics::Texture2D> m_p_texture;

            ECS::Entity m_entity;
    };
}