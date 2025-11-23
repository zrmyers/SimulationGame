#pragma once
#include "sdl/SDL.hpp"
#include <SDL3/SDL_gpu.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>

namespace Graphics {

    struct DrawCommand {
        // per draw resources.
        SDL_GPUTextureSamplerBinding textureSampler;

        //! Starting index
        uint32_t m_start_index;

        //! Vertex offset
        int32_t m_vertex_offset;

        //! Number of indices
        uint32_t m_num_indices;

        //! Starting instance.
        uint32_t m_start_instance;

        //! Number of instances.
        uint32_t m_num_instances;
    };

    struct Renderable {

            //! Whether the renderable is loaded. If not loaded, the transfer information must be used to upload data
            //! before drawing.
            bool is_loaded;

            //! Transfer information.
            SDL_GPUTransferBufferLocation m_vertex_src;
            SDL_GPUTransferBufferLocation m_index_src;
            SDL_GPUBufferRegion m_vertex_dst;
            SDL_GPUBufferRegion m_index_dst;

            //! Whether the renderable is ready for drawing.
            bool is_visible;

            //! Which pipeline to bind.
            SDL::GraphicsPipeline* m_p_pipeline;

            //! Which vertex buffer to bind.
            SDL_GPUBufferBinding m_vertex_buffer_binding;
            SDL_GPUBufferBinding m_index_buffer_binding;

            //! Which samplers to bind.
            std::vector<DrawCommand> m_drawcommands;
    };

}