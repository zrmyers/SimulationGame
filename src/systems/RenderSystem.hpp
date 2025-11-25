#pragma once

#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "graphics/ShaderCross.hpp"
#include "sdl/SDL.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>
#include <cstdint>

#include "glm/mat4x4.hpp"


namespace Systems {

    class RenderSystem : public ECS::System {

        public:

            //! Initialize the renderer.
            RenderSystem(Core::Engine& engine);
            RenderSystem(const RenderSystem& other) = delete;
            RenderSystem(RenderSystem&& other) = delete;
            RenderSystem& operator=(const RenderSystem& other) = delete;
            RenderSystem& operator=(RenderSystem&& other) = delete;
            ~RenderSystem() override;

            SDL::GpuDevice& GetGpuDevice();

            //! Get Swap chain texture format.
            SDL_GPUTextureFormat GetSwapchainTextureFormat();

            //! Create a shader.
            SDL::Shader CreateShader(Graphics::ByteCode& code);

            //! Create a pipeline for the GPU.
            SDL::GraphicsPipeline CreatePipeline(SDL_GPUGraphicsPipelineCreateInfo& pipeline_info);

            //! Create a buffer on the GPU.
            SDL::GpuBuffer CreateBuffer(SDL_GPUBufferUsageFlags usage, uint32_t size);

            //! Create a transfer buffer on the GPU.
            SDL::GpuTransferBuffer CreateTransferBuffer(SDL_GPUTransferBufferUsage usage, uint32_t size);

            //! Create a sampler on the GPU.
            SDL::GpuSampler CreateSampler(SDL_GPUSamplerCreateInfo& sampler_info);

            //! Main update loop.
            //!
            //! - perform transfers
            //! - draw
            void Update() override;

        private:

            //! Window used for rendering graphics to screen.
            SDL::Window m_window;

            //! Handle to GPU used for graphics processing
            SDL::GpuDevice m_gpu;
    };
}