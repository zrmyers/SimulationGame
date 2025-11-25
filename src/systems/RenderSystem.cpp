#include "RenderSystem.hpp"

#include "components/Renderable.hpp"
#include "core/Engine.hpp"
#include "core/Logger.hpp"
#include "ecs/ECS.hpp"
#include "graphics/ShaderCross.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3_ttf/SDL_ttf.h>
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

Systems::RenderSystem::RenderSystem(Core::Engine& engine)
    : ECS::System(engine)
    , m_window("Simulation Game", 1024, 768, SDL_WINDOW_VULKAN)
    , m_gpu(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL) {

    m_window.SetPosition( SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    Core::Logger::Info("OK: Created device with driver: " + std::string(m_gpu.GetDriver()));

    m_gpu.ClaimWindow(m_window);
}


Systems::RenderSystem::~RenderSystem() {
    m_gpu.ReleaseWindow(m_window);
}

SDL::GpuDevice& Systems::RenderSystem::GetGpuDevice() {
    return m_gpu;
}

SDL_GPUTextureFormat Systems::RenderSystem::GetSwapchainTextureFormat() {
    return m_gpu.GetSwapchainTextureFormat(m_window);
}

SDL::Shader Systems::RenderSystem::CreateShader(Graphics::ByteCode& code) {
    return SDL::Shader(code, m_gpu);
}

SDL::GraphicsPipeline Systems::RenderSystem::CreatePipeline(SDL_GPUGraphicsPipelineCreateInfo& pipeline_info) {
    return SDL::GraphicsPipeline(m_gpu, pipeline_info);
}

SDL::GpuBuffer Systems::RenderSystem::CreateBuffer(SDL_GPUBufferUsageFlags usage, uint32_t size) {
    SDL_GPUBufferCreateInfo createinfo = { usage, size, 0};
    return SDL::GpuBuffer(m_gpu, createinfo);
}

SDL::GpuTransferBuffer Systems::RenderSystem::CreateTransferBuffer(SDL_GPUTransferBufferUsage usage, uint32_t size) {
    SDL_GPUTransferBufferCreateInfo createinfo = {usage, size, 0};
    return SDL::GpuTransferBuffer(m_gpu, createinfo);
}

SDL::GpuSampler Systems::RenderSystem::CreateSampler(SDL_GPUSamplerCreateInfo& sampler_info) {
    return SDL::GpuSampler(m_gpu, sampler_info);
}


void Systems::RenderSystem::Update() {

    SDL_GPUCommandBuffer* p_cmdbuf = SDL_AcquireGPUCommandBuffer(m_gpu.Get());
    if (p_cmdbuf == nullptr) {
        throw SDL::Error("SDL_AcquireGPUCommandBuffer() failed! ");
    }

    SDL_GPUTexture* p_swapchainTexture = nullptr;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(p_cmdbuf, m_window.Get(), &p_swapchainTexture, nullptr, nullptr)) {
        throw SDL::Error("SDL_WaitAndAcquireGPUSwapchainTexture() failed!");
    }

    if (p_swapchainTexture != nullptr) {

        ECS::Registry& registry = GetEngine().GetEcsRegistry();
        std::set<ECS::EntityID_t> entities = GetEntities();

        SDL_GPUCopyPass* p_copyPass = SDL_BeginGPUCopyPass(p_cmdbuf);
        // Process uploads.
        for (ECS::EntityID_t entity : entities) {

            Components::Renderable& renderable = registry.GetComponent<Components::Renderable>(entity);
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

        for (ECS::EntityID_t entity : entities) {

            Components::Renderable& renderable = registry.GetComponent<Components::Renderable>(entity);
            if (renderable.is_loaded && renderable.is_visible) {

                SDL_BindGPUGraphicsPipeline(p_renderPass, renderable.m_p_pipeline->Get());
                SDL_BindGPUVertexBuffers(p_renderPass, 0, &renderable.m_vertex_buffer_binding, 1U);
                SDL_BindGPUIndexBuffer(p_renderPass, &renderable.m_index_buffer_binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
                SDL_PushGPUVertexUniformData(p_cmdbuf, 0U, &renderable.uniform_data, sizeof(renderable.uniform_data));

                for (Components::DrawCommand& command : renderable.m_drawcommands) {

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


