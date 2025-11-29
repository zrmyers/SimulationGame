#pragma once

#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "graphics/pipelines/PipelineCache.hpp"
#include "graphics/ShaderCross.hpp"
#include "sdl/SDL.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>
#include <cstdint>
#include <vector>

#include "graphics/UploadData.hpp"

#include "glm/mat4x4.hpp"


namespace Systems {

    class RenderSystem : public ECS::System {

        public:
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

            static uint32_t GetRequestLength(const TransferRequest& request) {

                uint32_t length = 0U;
                switch(request.type) {
                    case RequestType::UPLOAD_TO_BUFFER:
                        length = request.data.buffer.size;
                        break;
                    case RequestType::UPLOAD_TO_TEXTURE:
                        length = request.data.texture.w * request.data.texture.h * 4;
                        break;
                    default:
                        break;
                }
                return length;
            }

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

            //! Create a sampler on the GPU.
            SDL::GpuSampler CreateSampler(SDL_GPUSamplerCreateInfo& sampler_info);

            //! Creates a texture on the GPU.
            SDL::GpuTexture CreateTexture(SDL_GPUTextureCreateInfo& texture_info);

            //! Immediate upload of data to buffer.
            void UploadDataToBuffer(const std::vector<TransferRequest>& transfers);

            //!
            //! Main update loop.
            //!
            //! - perform transfers
            //! - draw
            void Update() override;

        private:


            //! Create a transfer buffer on the GPU.
            SDL::GpuTransferBuffer CreateTransferBuffer(SDL_GPUTransferBufferUsage usage, uint32_t size);

            void ProcessDataUpload(SDL_GPUCopyPass* p_copypass, const std::vector<TransferRequest>& transfers);

            //! Window used for rendering graphics to screen.
            SDL::Window m_window;

            //! Handle to GPU used for graphics processing
            SDL::GpuDevice m_gpu;

            //! Pipeline Cache
            Graphics::PipelineCache m_pipeline_cache;

            //! Buffer used for transferring data to GPU.
            SDL::GpuTransferBuffer m_transfer_buffer;
    };
}