#pragma once

#include "components/Renderable.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "graphics/pipelines/PipelineCache.hpp"
#include "graphics/ShaderCross.hpp"
#include "graphics/Mesh.hpp"
#include "sdl/SDL.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>
#include <cstddef>
#include <cstdint>
#include <glm/ext/vector_int2.hpp>
#include <vector>

#include "graphics/UploadData.hpp"

#include "glm/mat4x4.hpp"


namespace Systems {

    class RenderSystem : public ECS::System {

        public:

            static uint32_t GetRequestLength(const Components::TransferRequest& request);

            //! Initialize the renderer.
            RenderSystem(Core::Engine& engine);
            RenderSystem(const RenderSystem& other) = delete;
            RenderSystem(RenderSystem&& other) = delete;
            RenderSystem& operator=(const RenderSystem& other) = delete;
            RenderSystem& operator=(RenderSystem&& other) = delete;
            ~RenderSystem() override;

            //! Enable or disable VSYNC
            void SetVsync(bool vsync_enabled);

            //! Enable or disable fullscreen
            void SetWindowMode(bool fullscreen, glm::ivec2);

            SDL::GpuDevice& GetGpuDevice();

            glm::ivec2 GetWindowSize() const;

            //! Get Swap chain texture format.
            SDL_GPUTextureFormat GetSwapchainTextureFormat();

            //! Create a shader.
            SDL::Shader CreateShader(Graphics::ByteCode& code);

            //! Create a pipeline for the GPU. Creates a pipeline that will be owned by the user.
            SDL::GraphicsPipeline CreatePipeline(SDL_GPUGraphicsPipelineCreateInfo& pipeline_info);

            //! Create a pipeline for the GPU. Finds or creates a pipeline from the shader cache.
            template<typename T>
            Graphics::IPipeline* CreatePipeline() {
                return m_pipeline_cache.Build<T>(
                    *this,
                    GetEngine().GetAssetLoader().GetShaderDir());
            }

            //! Create a buffer on the GPU.
            SDL::GpuBuffer CreateBuffer(SDL_GPUBufferUsageFlags usage, uint32_t size);

            //! Create a mesh
            Graphics::Mesh CreateMesh(size_t vertex_size, size_t num_vertices, SDL_GPUIndexElementSize index_size, size_t num_indices);

            //! Create a sampler on the GPU.
            SDL::GpuSampler CreateSampler(SDL_GPUSamplerCreateInfo& sampler_info);

            //! Creates a texture on the GPU.
            SDL::GpuTexture CreateTexture(SDL_GPUTextureCreateInfo& texture_info);

            //! Immediate upload of data to buffer.
            void UploadDataToBuffer(const std::vector<Components::TransferRequest>& transfers);

            //! Generate mipmaps for the given texture image.
            void GenerateMipMaps(SDL::GpuTexture& texture);

            //!
            //! Main update loop.
            //!
            //! - perform transfers
            //! - draw
            void Update() override;

        private:

            std::list<Components::Renderable*> SortDrawCalls();

            //! Create a transfer buffer on the GPU.
            SDL::GpuTransferBuffer CreateTransferBuffer(SDL_GPUTransferBufferUsage usage, uint32_t size);

            static void ProcessDataUpload(SDL_GPUCopyPass* p_copypass, const std::vector<Components::TransferRequest>& transfers, SDL::GpuTransferBuffer& transferBuffer, bool cycle);

            //! Window used for rendering graphics to screen.
            SDL::Window m_window;

            //! Handle to GPU used for graphics processing
            SDL::GpuDevice m_gpu;

            //! Pipeline Cache
            Graphics::PipelineCache m_pipeline_cache;

            //! Buffer used for transferring data to GPU.
            SDL::GpuTransferBuffer m_async_transfer_buffer;
            SDL::GpuTransferBuffer m_sync_transfer_buffer;

            //! Fence for synchronous gpu upload.
            SDL::GpuFence m_sync_transfer_fence;

            //! Texture for Depth pass.
            SDL::GpuTexture m_depth_texture;
    };
}