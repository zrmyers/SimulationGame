#pragma once
#include "sdl/SDL.hpp"
#include <SDL3/SDL_gpu.h>
#include <cstdint>
#include <vector>
#include <glm/mat4x4.hpp>
#include "graphics/pipelines/PipelineCache.hpp"

namespace Components {

    // Which render layer the object is rendered to.
    enum class RenderLayer : uint8_t {
        LAYER_NONE = 0, // The object is invisible
        LAYER_3D_OPAQUE,
        LAYER_3D_TRANSPARENT,
        LAYER_GUI
    };

    enum class RequestType : uint8_t {
        UPLOAD_TO_BUFFER,
        UPLOAD_TO_TEXTURE
    };

    union RequestData {
        SDL_GPUBufferRegion buffer;
        SDL_GPUTextureRegion texture;
    };

    struct TransferRequest {
        RequestType type;
        RequestData data;
        void* p_src;
        bool cycle;
    };

    // @nested part of Renderable
    struct DrawCommand {

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
            RenderLayer m_layer;

            //! Used for manual draw call sorting. A value of 0 is invalid and means depth should be obtained from transform+reference point.
            uint16_t m_depth_override;

            //! Which pipeline to bind.
            Graphics::IPipeline* m_p_pipeline;

            //! Which vertex buffer to bind.
            SDL_GPUBufferBinding m_vertex_buffer_binding;
            SDL_GPUBufferBinding m_index_buffer_binding;
            SDL_GPUIndexElementSize m_index_size;

            //! Any transfers to execute.
            std::vector<TransferRequest> m_requests;

            // per draw resources.
            SDL_GPUTextureSamplerBinding textureSampler;

            // transform
            glm::mat4 transform;

            //! Which samplers to bind.
            DrawCommand m_drawcommand;

            void Reset() {
                m_layer = RenderLayer::LAYER_NONE;
                m_depth_override = 0U;

                // material
                m_p_pipeline = nullptr;

                // mesh
                m_vertex_buffer_binding = {};
                m_index_buffer_binding = {};
                m_index_size = SDL_GPU_INDEXELEMENTSIZE_32BIT;

                // upload
                m_requests.clear();

                // draw
                m_drawcommand = {};
            }
    };

}