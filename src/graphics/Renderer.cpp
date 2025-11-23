#include "Renderer.hpp"

#include "Renderable.hpp"
#include "core/AssetLoader.hpp"
#include "core/Logger.hpp"
#include "ShaderCross.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>
#include <vector>

#include "sdl/SDL.hpp"
#include "sdl/TTF.hpp"


Graphics::Renderer::Renderer(SDL::Context& context)
    : m_window(context, "Simulation Game", 1024, 768, SDL_WINDOW_VULKAN)
    , m_gpu(context, SDL_GPU_SHADERFORMAT_SPIRV, true, NULL) {

    m_window.SetPosition( SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    Core::Logger::Info("OK: Created device with driver: " + std::string(m_gpu.GetDriver()));

    m_gpu.ClaimWindow(m_window);
}


Graphics::Renderer::~Renderer() {
    m_gpu.ReleaseWindow(m_window);
}

SDL::GpuDevice& Graphics::Renderer::GetGpuDevice() {
    return m_gpu;
}

SDL_GPUTextureFormat Graphics::Renderer::GetSwapchainTextureFormat() {
    return m_gpu.GetSwapchainTextureFormat(m_window);
}

SDL::Shader Graphics::Renderer::CreateShader(Graphics::ByteCode& code) {
    return SDL::Shader(code, m_gpu);
}

SDL::GraphicsPipeline Graphics::Renderer::CreatePipeline(SDL_GPUGraphicsPipelineCreateInfo& pipeline_info) {
    return SDL::GraphicsPipeline(m_gpu, pipeline_info);
}

SDL::GpuBuffer Graphics::Renderer::CreateBuffer(SDL_GPUBufferUsageFlags usage, uint32_t size) {
    SDL_GPUBufferCreateInfo createinfo = { usage, size, 0};
    return SDL::GpuBuffer(m_gpu, createinfo);
}

SDL::GpuTransferBuffer Graphics::Renderer::CreateTransferBuffer(SDL_GPUTransferBufferUsage usage, uint32_t size) {
    SDL_GPUTransferBufferCreateInfo createinfo = {usage, size, 0};
    return SDL::GpuTransferBuffer(m_gpu, createinfo);
}

SDL::GpuSampler Graphics::Renderer::CreateSampler(SDL_GPUSamplerCreateInfo& sampler_info) {
    return SDL::GpuSampler(m_gpu, sampler_info);
}


void Graphics::Renderer::Update(std::vector<Renderable>& renderables, const Graphics::UniformData& uniform) {

    SDL_GPUCommandBuffer* p_cmdbuf = SDL_AcquireGPUCommandBuffer(m_gpu.Get());
    if (p_cmdbuf == nullptr) {
        throw SDL::Error("SDL_AcquireGPUCommandBuffer() failed! ");
    }

    SDL_GPUTexture* p_swapchainTexture = nullptr;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(p_cmdbuf, m_window.Get(), &p_swapchainTexture, nullptr, nullptr)) {
        throw SDL::Error("SDL_WaitAndAcquireGPUSwapchainTexture() failed!");
    }

    if (p_swapchainTexture != nullptr) {

        SDL_GPUCopyPass* p_copyPass = SDL_BeginGPUCopyPass(p_cmdbuf);
        // Process uploads.
        for (Renderable& renderable : renderables) {

            if (!renderable.is_loaded) {

                // Process transfer.
                SDL_UploadToGPUBuffer(p_copyPass, &renderable.m_vertex_src, &renderable.m_vertex_dst, false);

                SDL_UploadToGPUBuffer(p_copyPass, &renderable.m_index_src, &renderable.m_index_dst, false);

                renderable.is_loaded = true;
            }
        }
        SDL_EndGPUCopyPass(p_copyPass);

        SDL_GPUColorTargetInfo colorTargetInfo = {0};
        colorTargetInfo.texture = p_swapchainTexture;
        colorTargetInfo.clear_color = SDL_FColor{0.0, 0.0, 0.0, 1.0};
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        // Draw commands
        SDL_GPURenderPass* p_renderPass = SDL_BeginGPURenderPass(p_cmdbuf, &colorTargetInfo, 1U, nullptr);

        SDL_PushGPUVertexUniformData(p_cmdbuf, 0U, &uniform, sizeof(uniform));

        for (Renderable& renderable : renderables) {

            if (renderable.is_loaded && renderable.is_visible) {

                SDL_BindGPUGraphicsPipeline(p_renderPass, renderable.m_p_pipeline->Get());
                SDL_BindGPUVertexBuffers(p_renderPass, 0, &renderable.m_vertex_buffer_binding, 1U);
                SDL_BindGPUIndexBuffer(p_renderPass, &renderable.m_index_buffer_binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

                for (DrawCommand& command : renderable.m_drawcommands) {

                    SDL_BindGPUFragmentSamplers(p_renderPass, 0U, &command.textureSampler, 1U);
                    SDL_DrawGPUIndexedPrimitives(
                        p_renderPass,
                        command.m_num_indices,
                        command.m_num_instances,
                        command.m_start_index,
                        command.m_vertex_offset,
                        command.m_start_instance);
                }
            }
        }

        SDL_EndGPURenderPass(p_renderPass);
    }

    SDL_SubmitGPUCommandBuffer(p_cmdbuf);
}


