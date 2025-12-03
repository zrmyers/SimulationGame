#pragma once

#include "core/Engine.hpp"
#include "sdl/SDL.hpp"
#include "systems/RenderSystem.hpp"
#include <SDL3/SDL_gpu.h>
#include <cstdint>
#include <memory>

namespace Graphics {
    class Texture2D {

        public:

            //! Constructor for a 2D texture.
            Texture2D(
                Core::Engine& engine,
                std::shared_ptr<SDL::GpuSampler> p_sampler,
                uint32_t width,
                uint32_t height,
                bool generate_mipmap);

            //! Uploads image data to the texture.
            void LoadImageData(const SDL::Image& image, glm::ivec2 dst_offset = {0, 0});

            uint32_t GetWidth() const;
            uint32_t GetHeight() const;
            bool HasMipMaps() const;

            SDL_GPUTextureSamplerBinding GetBinding();

        private:

            // RenderSystem reference
            Systems::RenderSystem* m_p_render_system;

            // sampler reference
            std::shared_ptr<SDL::GpuSampler> m_p_sampler;

            // texture reference
            SDL::GpuTexture m_texture;

            // width
            uint32_t m_width;

            // height
            uint32_t m_height;

            // bool generate mipmaps
            bool m_mipmaps;
    };

}