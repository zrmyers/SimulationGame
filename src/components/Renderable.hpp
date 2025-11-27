#pragma once
#include "sdl/SDL.hpp"
#include <SDL3/SDL_gpu.h>
#include <cstdint>
#include <vector>
#include <glm/mat4x4.hpp>
#include "graphics/pipelines/PipelineCache.hpp"
#include "systems/RenderSystem.hpp"
namespace Components {

    //! Uniform data.
    struct UniformData {
        glm::mat4 projview{};
        glm::mat4 model{};
    };

    // @nested part of Renderable
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

            //! Whether the renderable is ready for drawing.
            bool is_visible;

            //! Vertex uniform data.
            UniformData uniform_data; // todo, this should come from camera in render system. model data should be stored in seperate buffer.

            //! Which pipeline to bind.
            Graphics::IPipeline* m_p_pipeline;

            //! Which vertex buffer to bind.
            SDL_GPUBufferBinding m_vertex_buffer_binding;
            SDL_GPUBufferBinding m_index_buffer_binding;
            SDL_GPUIndexElementSize m_index_size;

            //! Any transfers to execute.
            std::vector<Systems::RenderSystem::TransferRequest> m_requests;

            //! Which samplers to bind.
            std::vector<DrawCommand> m_drawcommands;
    };

}