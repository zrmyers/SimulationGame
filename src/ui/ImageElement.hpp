#pragma once

#include "Element.hpp"
#include "ecs/ECS.hpp"
#include "graphics/Texture2D.hpp"

namespace UI {

        //! An element that contains a single image.
    class ImageElement : public Element {

        public:

            ImageElement();

            ImageElement& SetTexture(std::shared_ptr<Graphics::Texture2D> p_texture);

            void UpdateGraphics(ECS::Registry& registry, glm::vec2 screenSize, int depth) override;
        private:

            std::shared_ptr<Graphics::Texture2D> m_p_texture;

            ECS::Entity m_entity;
    };
}