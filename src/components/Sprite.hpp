#pragma once

#include "Renderable.hpp"
#include "sdl/SDL.hpp"
#include "glm/vec2.hpp"
#include "glm/vec4.hpp"
#include <memory>

namespace Components {

    // information used by the sprite system to build sprites.
    struct Sprite {

        // The texture used for the sprite.
        std::shared_ptr<SDL::GpuTexture> texture;

        // The sampler used for the sprite.
        std::shared_ptr<SDL::GpuSampler> sampler;

        // The UV coordinate at the top-left corner of the sprite. Used to index into a texture atlas.
        glm::vec2 topLeftUV{0.0F, 0.0F};

        // The UV coordinate at the bottom-right corner of the sprite.
        glm::vec2 bottomRightUV{1.0F, 1.0F};

        // The UV coordinate at the top-right corner of the sprite.
        // The color to apply to the sprite.
        glm::vec4 color {1.0F, 1.0F, 1.0F, 1.0F};

        // Which layer the sprite should be rendered to.
        RenderLayer layer {RenderLayer::LAYER_NONE};
    };

}