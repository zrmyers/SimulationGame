#include "RenderSystem.hpp"

#include "components/Camera.hpp"
#include "components/Renderable.hpp"
#include "core/Engine.hpp"
#include "core/Logger.hpp"
#include "core/Settings.hpp"
#include "ecs/ECS.hpp"
#include "graphics/ShaderCross.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>
#include <unordered_map>
#include <vector>

#include "sdl/SDL.hpp"

struct PerVertexUniformData {
    glm::mat4 projView;
    glm::mat4 model;
};

// Total number of bytes to allocate for a transfer buffer.
static constexpr uint32_t TRANSFER_BUFFER_SIZE = 128U * 1024U * 1024;

// Bits used for generating key for sorting renderables.
static constexpr uint32_t RENDER_LAYER_MASK = 0xFF000000U;
static constexpr uint32_t RENDER_LAYER_SHIFT = 24U;

static constexpr uint32_t RENDER_DEPTH_MASK = 0x00FFFFFFU;
static constexpr uint32_t RENDER_DEPTH_SHIFT = 0U;

uint32_t Systems::RenderSystem::GetRequestLength(const Components::TransferRequest &request) {

    uint32_t length = 0U;
    switch(request.type) {
        case Components::RequestType::UPLOAD_TO_BUFFER:
            length = request.data.buffer.size;
            break;
        case Components::RequestType::UPLOAD_TO_TEXTURE:
            length = request.data.texture.w * request.data.texture.h * 4;
            break;
        default:
            break;
    }
    return length;
}


Systems::RenderSystem::RenderSystem(Core::Engine& engine)
    : ECS::System(engine)
    , m_gpu(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL) {

    Core::Logger::Info("OK: Created device with driver: " + std::string(m_gpu.GetDriver()));

    Core::GraphicsSettings& graphics = engine.GetSettings().GetGraphicsSettings();
    SDL_WindowFlags windowFlags = graphics.GetFullscreen()? SDL_WINDOW_FULLSCREEN : 0;
    windowFlags |= SDL_WINDOW_VULKAN;
    glm::ivec2 resolution = graphics.GetDisplayResolution();

    m_window = SDL::Window("Simulation Game", resolution.x, resolution.y, windowFlags);

    Core::Logger::Info("OK: Created window");
    m_window.SetPosition( SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    m_gpu.ClaimWindow(m_window);

    SetVsync(graphics.GetVsyncEnabled());

    m_transfer_buffer = CreateTransferBuffer(SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, TRANSFER_BUFFER_SIZE);
}

void Systems::RenderSystem::SetVsync(bool vsync_enabled) {

    SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_IMMEDIATE;

    bool supportsMailbox = SDL_WindowSupportsGPUPresentMode(m_gpu.Get(), m_window.Get(), SDL_GPU_PRESENTMODE_MAILBOX);
    if (vsync_enabled) {

        presentMode = supportsMailbox? SDL_GPU_PRESENTMODE_MAILBOX : SDL_GPU_PRESENTMODE_VSYNC;
    }

    if (!SDL_SetGPUSwapchainParameters(m_gpu.Get(), m_window.Get(), SDL_GPU_SWAPCHAINCOMPOSITION_SDR, presentMode)) {
        throw SDL::Error("SDL_SetGPUSwapchainParameters() failed!");
    }
}

void Systems::RenderSystem::SetFullscreen(bool fullscreen) {

    m_window.SetFullscreen(fullscreen);
}

Systems::RenderSystem::~RenderSystem() {
    m_gpu.ReleaseWindow(m_window);
}

SDL::GpuDevice& Systems::RenderSystem::GetGpuDevice() {
    return m_gpu;
}

glm::ivec2 Systems::RenderSystem::GetWindowSize() const {
    return m_window.GetWindowSize();
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

SDL::GpuTexture Systems::RenderSystem::CreateTexture(SDL_GPUTextureCreateInfo& texture_info) {
    return SDL::GpuTexture(m_gpu, texture_info);
}

void Systems::RenderSystem::UploadDataToBuffer(const std::vector<Components::TransferRequest>& transfers) {

    SDL_GPUCommandBuffer* p_cmd = SDL_AcquireGPUCommandBuffer(m_gpu.Get());
    if (p_cmd == nullptr) {
        throw SDL::Error("SDL_AcquireGPUCommandBuffer() failed!");
    }
    SDL_GPUCopyPass* p_copy = SDL_BeginGPUCopyPass(p_cmd);

    ProcessDataUpload(p_copy, transfers);

    SDL_EndGPUCopyPass(p_copy);
    if (!SDL_SubmitGPUCommandBuffer(p_cmd)) {
        throw SDL::Error("SDL_SubmitGPUCommandBuffer() failed!");
    }
}

void Systems::RenderSystem::GenerateMipMaps(SDL::GpuTexture& texture) {

    SDL_GPUCommandBuffer* p_cmd = SDL_AcquireGPUCommandBuffer(m_gpu.Get());
    if (p_cmd == nullptr) {
        throw SDL::Error("SDL_GPUCommandBuffer() failed!");
    }

    SDL_GenerateMipmapsForGPUTexture(p_cmd, texture.Get());

    if(!SDL_SubmitGPUCommandBuffer(p_cmd)) {
        throw SDL::Error("SDL_SubmitGPUCommandBuffer() failed!");
    }
}

void Systems::RenderSystem::Update() {

    // todo frustrum culling
    std::list<Components::Renderable*> renderables = SortDrawCalls();

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
        const auto& cameras = registry.GetComponentArray<Components::Camera>();

        SDL_GPUCopyPass* p_copyPass = SDL_BeginGPUCopyPass(p_cmdbuf);
        // Process uploads.
        for (Components::Renderable* p_renderable : renderables) {

            if (!p_renderable->m_requests.empty()) {

                // Process transfer.
                ProcessDataUpload(p_copyPass, p_renderable->m_requests);
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

        for (Components::Renderable* p_renderable : renderables) {

            Components::Renderable& renderable = *p_renderable;
            if (renderable.m_layer == Components::RenderLayer::LAYER_GUI) {
                PerVertexUniformData uniform = {};
                uniform.model = renderable.transform;
                uniform.projView = glm::mat4(1.0F); // orthrographic camera

                SDL_BindGPUGraphicsPipeline(p_renderPass, renderable.m_p_pipeline->Get().Get());
                SDL_BindGPUVertexBuffers(p_renderPass, 0, &renderable.m_vertex_buffer_binding, 1U);
                SDL_BindGPUIndexBuffer(p_renderPass, &renderable.m_index_buffer_binding, renderable.m_index_size);

                SDL_PushGPUVertexUniformData(p_cmdbuf, 0U, &uniform, sizeof(uniform));

                SDL_BindGPUFragmentSamplers(p_renderPass, 0U, &renderable.textureSampler, 1U);

                Components::DrawCommand& command = renderable.m_drawcommand;
                SDL_DrawGPUIndexedPrimitives(
                    p_renderPass,
                    command.m_num_indices,
                    command.m_num_instances,
                    command.m_start_index,
                    command.m_vertex_offset,
                    command.m_start_instance);
            } else if ((renderable.m_layer == Components::RenderLayer::LAYER_3D_OPAQUE)
                && (cameras->GetSize() > 0)) {

                const Components::Camera& camera = cameras->GetByIndex(0U); // orthrographic camera

                PerVertexUniformData uniform = {};
                uniform.model = renderable.transform;
                uniform.projView = camera.GetProjection() * camera.GetView();

                SDL_BindGPUGraphicsPipeline(p_renderPass, renderable.m_p_pipeline->Get().Get());
                SDL_BindGPUVertexBuffers(p_renderPass, 0, &renderable.m_vertex_buffer_binding, 1U);
                SDL_BindGPUIndexBuffer(p_renderPass, &renderable.m_index_buffer_binding, renderable.m_index_size);

                SDL_PushGPUVertexUniformData(p_cmdbuf, 0U, &uniform, sizeof(uniform));

                SDL_BindGPUFragmentSamplers(p_renderPass, 0U, &renderable.textureSampler, 1U);

                Components::DrawCommand& command = renderable.m_drawcommand;
                SDL_DrawGPUIndexedPrimitives(
                    p_renderPass,
                    command.m_num_indices,
                    command.m_num_instances,
                    command.m_start_index,
                    command.m_vertex_offset,
                    command.m_start_instance);
            }
        }

        SDL_EndGPURenderPass(p_renderPass);
    }

    SDL_SubmitGPUCommandBuffer(p_cmdbuf);
}


std::list<Components::Renderable*> Systems::RenderSystem::SortDrawCalls() {

    std::list<Components::Renderable*> renderables;
    std::unordered_map<uint32_t, std::list<Components::Renderable*>> binnedRenderables;
    std::vector<uint32_t> sortKeys;

    ECS::Registry& registry = GetEngine().GetEcsRegistry();
    std::set<ECS::EntityID_t> entities = GetEntities();
    sortKeys.reserve(entities.size());

    for (ECS::EntityID_t entityId : entities) {
        Components::Renderable& renderable = registry.GetComponent<Components::Renderable>(entityId);

        if (renderable.m_layer == Components::RenderLayer::LAYER_NONE) {

            // skip renderables that are not currently visible.
            continue;
        }

        uint32_t sortKey = static_cast<uint8_t>(renderable.m_layer) << RENDER_LAYER_SHIFT;

        // calculate depth
        if (renderable.m_depth_override > 0U) {
            sortKey |= (static_cast<uint32_t>(renderable.m_depth_override) & RENDER_DEPTH_MASK);
        }
        else {

            // todo use origin + transform and camera information to calculate depth value
        }

        binnedRenderables[sortKey].push_back(&renderable);
        sortKeys.push_back(sortKey);
    }

    std::sort(sortKeys.begin(), sortKeys.end(),[](const uint32_t& left, uint32_t& right){
        return left < right;
    });

    // now build list by splicing lists by keys
    for (uint32_t key : sortKeys) {

        renderables.splice(renderables.end(), binnedRenderables.at(key));
    }

    return renderables;
}

void Systems::RenderSystem::ProcessDataUpload(SDL_GPUCopyPass* p_copypass, const std::vector<Components::TransferRequest>& transfers) {

    uint32_t transferOffset = 0U;

    // map the transfer buffer.  Cycle to ensure that we don't overwrite transfer buffers that are currently bound.
    uint8_t* p_data =static_cast<uint8_t*>( m_transfer_buffer.Map(true));

    // copy data to transfer buffer
    for (const Components::TransferRequest& request : transfers) {

        uint32_t data_len = GetRequestLength(request);

        if (data_len + transferOffset > TRANSFER_BUFFER_SIZE) {
            Core::Logger::Error("Transfer request exceeds max buffer size. skipping buffer.");
            continue;
        }

        SDL_memcpy(p_data + transferOffset, request.p_src, data_len);

        if (request.type == Components::RequestType::UPLOAD_TO_BUFFER) {
            SDL_GPUTransferBufferLocation src = {m_transfer_buffer.Get(), transferOffset};

            SDL_UploadToGPUBuffer(p_copypass, &src, &request.data.buffer, request.cycle);
        }
        else if (request.type == Components::RequestType::UPLOAD_TO_TEXTURE) {
            SDL_GPUTextureTransferInfo src = {m_transfer_buffer.Get(), transferOffset, 0U, 0U};

            SDL_UploadToGPUTexture(p_copypass, &src, &request.data.texture, request.cycle);
        }

        transferOffset += data_len;
    }

    m_transfer_buffer.Unmap();
}