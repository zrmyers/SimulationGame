#pragma once

#include "RenderSystem.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "graphics/pipelines/PipelineCache.hpp"
#include "graphics/pipelines/UnlitTexturePipeline.hpp"
#include "sdl/SDL.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

namespace Systems {

    //! The sprite system consumes sprite components and produces renderables that can be consumed by the render system.
    class SpriteSystem : public ECS::System {

        public:

            //! A constructor for the sprite system.
            SpriteSystem(Core::Engine& engine);

            //! The main update function.
            void Update() override;

        private:

            void UploadData(Systems::RenderSystem& rendersystem);

            // Pipeline for rendering sprites.
            Graphics::IPipeline* m_p_sprite_pipeline;

            // sampler
            SDL::GpuSampler m_sampler;

            // mesh buffers
            SDL::GpuBuffer m_vertex_buffer;
            SDL::GpuBuffer m_index_buffer;

            //! Transfer Buffer. Use for uploading data to GPU.
            SDL::GpuTransferBuffer m_bufferTransferbuffer;
            SDL::GpuTransferBuffer m_textureTransferbuffer;

            //! Geometry data.
            std::vector<Graphics::UnlitTexturedVertex> m_vertices;
            std::vector<Uint16> m_indices;
    };
}