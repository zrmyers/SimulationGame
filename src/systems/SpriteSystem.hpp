#pragma once

#include "components/Renderable.hpp"
#include "components/Transform.hpp"
#include "components/Sprite.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "graphics/pipelines/PipelineCache.hpp"
#include "graphics/pipelines/UnlitTexturePipeline.hpp"
#include "sdl/SDL.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <unordered_map>

namespace Systems {

    //! The sprite system consumes sprite components and produces renderables that can be consumed by the render system.
    class SpriteSystem : public ECS::System {

        public:

            //! A constructor for the sprite system.
            SpriteSystem(Core::Engine& engine);

            //! The main update function.
            void Update() override;

        private:

            // Add a sprite to the geometry buffers.
            void InputSprite(const Components::Sprite& sprite, const Components::Transform& transform);
            void OutputBatch(
                Components::Renderable& renderable,
                SDL_GPUTexture* p_texture,
                uint32_t vertexOffset,
                uint32_t numVertices,
                uint32_t indexOffset,
                uint32_t numIndices);

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

            //! Sprite Batches
            std::unordered_map<SDL_GPUTexture*, ECS::Entity> m_sprite_batches;
    };
}