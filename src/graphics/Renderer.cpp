#include "Renderer.hpp"

#include "core/AssetLoader.hpp"
#include "core/Logger.hpp"
#include "ShaderCross.hpp"
#include <SDL3/SDL_gpu.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "sdl/SDL.hpp"

static const constexpr uint32_t MAX_VERTEX_COUNT = 4000;
static const constexpr uint32_t MAX_INDEX_COUNT = 6000;

Graphics::Renderer::Renderer(SDL::Context& context, Core::AssetLoader& asset_loader)
    : m_window(context, "Simulation Game", 1024, 768, SDL_WINDOW_VULKAN)
    , m_gpu(context, SDL_GPU_SHADERFORMAT_SPIRV, true, NULL) {

    m_window.SetPosition( SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    Core::Logger::Info("OK: Created device with driver: " + std::string(m_gpu.GetDriver()));

    m_gpu.ClaimWindow(m_window);

    BuildSimplePipeline(asset_loader.GetShaderDir());
    Build3DTextPipeline(asset_loader.GetShaderDir());

    SDL_GPUBufferCreateInfo vbf_info = {SDL_GPU_BUFFERUSAGE_VERTEX, sizeof(Vertex) * MAX_VERTEX_COUNT };
    m_vertexbuffer = SDL::GpuBuffer(m_gpu,vbf_info);

    SDL_GPUBufferCreateInfo ibf_info = {SDL_GPU_BUFFERUSAGE_INDEX, sizeof(int) * MAX_INDEX_COUNT};
    m_indexbuffer = SDL::GpuBuffer(m_gpu, ibf_info);

    SDL_GPUTransferBufferCreateInfo tbf_info = {SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, vbf_info.size + ibf_info.size};
    m_transferbuffer = SDL::GpuTransferBuffer(m_gpu, tbf_info);

    SDL_GPUSamplerCreateInfo sampler_info = {};
    sampler_info.min_filter = SDL_GPU_FILTER_LINEAR;
    sampler_info.mag_filter = SDL_GPU_FILTER_LINEAR,
    sampler_info.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sampler_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

    m_sampler = SDL::GpuSampler(m_gpu, sampler_info);

    m_geometry_data.vertices.resize(MAX_VERTEX_COUNT);
    m_geometry_data.indices.resize(MAX_INDEX_COUNT);

}

Graphics::Renderer::~Renderer() {
    m_gpu.ReleaseWindow(m_window);
}

void Graphics::Renderer::Draw() {

    SDL_GPUCommandBuffer* p_cmdbuf = SDL_AcquireGPUCommandBuffer(m_gpu.Get());
    if (p_cmdbuf == nullptr) {
        throw SDL::Error("SDL_AcquireGPUCommandBuffer() failed! ");
    }

    SDL_GPUTexture* p_swapchainTexture = nullptr;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(p_cmdbuf, m_window.Get(), &p_swapchainTexture, nullptr, nullptr)) {
        throw SDL::Error("SDL_WaitAndAcquireGPUSwapchainTexture() failed!");
    }

    if (p_swapchainTexture != nullptr) {

        SDL_GPUColorTargetInfo colorTargetInfo = {0};
        colorTargetInfo.texture = p_swapchainTexture;
        colorTargetInfo.clear_color = SDL_FColor{0.0, 0.0, 0.0, 1.0};
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* p_renderPass = SDL_BeginGPURenderPass(p_cmdbuf, &colorTargetInfo, 1U, nullptr);
        SDL_BindGPUGraphicsPipeline(p_renderPass, m_fillpipeline.Get());

        SDL_DrawGPUPrimitives(p_renderPass, 3, 1, 0, 0);
        SDL_EndGPURenderPass(p_renderPass);
    }

    SDL_SubmitGPUCommandBuffer(p_cmdbuf);
}

void Graphics::Renderer::BuildSimplePipeline(const std::string& shader_path) {

    ShaderCross compiler;

    // Load file from filesystem
    Graphics::ByteCode vertexShaderBytecode = compiler.CompileToSpirv(
        EShLangVertex, shader_path + "/simple.vert.glsl", "main", shader_path, {});

    SDL::Shader vertexShader(vertexShaderBytecode, m_gpu);

    Graphics::ByteCode fragmentShaderBytecode = compiler.CompileToSpirv(
        EShLangFragment, shader_path + "/simple.frag.glsl", "main", shader_path, {});

    SDL::Shader fragmentShader(fragmentShaderBytecode, m_gpu);

    std::vector<SDL_GPUColorTargetDescription> descriptions = {
        {m_gpu.GetSwapchainTextureFormat(m_window)}};

    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.target_info.num_color_targets = 1;
    pipelineCreateInfo.target_info.color_target_descriptions = descriptions.data();
    pipelineCreateInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineCreateInfo.vertex_shader = vertexShader.Get();
    pipelineCreateInfo.fragment_shader = fragmentShader.Get();
    pipelineCreateInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;

    m_fillpipeline = SDL::GraphicsPipeline(
        m_gpu,
        pipelineCreateInfo);

    pipelineCreateInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_LINE;
    m_wirepipeline = SDL::GraphicsPipeline(
        m_gpu,
        pipelineCreateInfo
    );
}

void Graphics::Renderer::Build3DTextPipeline(const std::string& shader_path) {

    ShaderCross compiler;

    // Load file from filesystem
    Graphics::ByteCode vertexShaderBytecode = compiler.CompileToSpirv(
        EShLangVertex, shader_path + "/guiText.vert.glsl", "main", shader_path, {});

    SDL::Shader vertexShader(vertexShaderBytecode, m_gpu);

    Graphics::ByteCode fragmentShaderBytecode = compiler.CompileToSpirv(
        EShLangFragment, shader_path + "/guiText.frag.glsl", "main", shader_path, {});

    SDL::Shader fragmentShader(fragmentShaderBytecode, m_gpu);

    SDL_GPUGraphicsPipelineCreateInfo pipelinecreateinfo = {};

    //------------------------------------------------------------------------------------------------------------------
    // Target Info Description

    SDL_GPUGraphicsPipelineTargetInfo& targetInfo = pipelinecreateinfo.target_info;

    SDL_GPUColorTargetDescription colorTargetDescription;
    colorTargetDescription.format = m_gpu.GetSwapchainTextureFormat(m_window);
    colorTargetDescription.blend_state.enable_blend = true,
    colorTargetDescription.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD,
    colorTargetDescription.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD,
    colorTargetDescription.blend_state.color_write_mask = 0xF, // NOLINT
    colorTargetDescription.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
    colorTargetDescription.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_DST_ALPHA,
    colorTargetDescription.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
    colorTargetDescription.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

    targetInfo.num_color_targets = 1;
    targetInfo.color_target_descriptions = & colorTargetDescription;
    targetInfo.has_depth_stencil_target = false;
    targetInfo.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_INVALID;

    //------------------------------------------------------------------------------------------------------------------
    // Vertex Input State
    SDL_GPUVertexInputState& vertexInputState = pipelinecreateinfo.vertex_input_state;
    // all attributes are interleaved in same structure
    SDL_GPUVertexBufferDescription vertexBufferDescription = {};
    vertexBufferDescription.slot = 0;
    vertexBufferDescription.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDescription.instance_step_rate = 0;
    vertexBufferDescription.pitch = sizeof(Vertex);

    vertexInputState.vertex_buffer_descriptions = &vertexBufferDescription;
    vertexInputState.num_vertex_buffers = 1;

    std::array<SDL_GPUVertexAttribute, 3> vertexAttributes = {};
    vertexAttributes[0].buffer_slot = 0;
    vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    vertexAttributes[0].location = 0;
    vertexAttributes[0].offset = 0;
    vertexAttributes[1].buffer_slot = 0;
    vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    vertexAttributes[1].location = 1;
    vertexAttributes[1].offset = offsetof(Vertex, color);
    vertexAttributes[2].buffer_slot = 0;
    vertexAttributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    vertexAttributes[2].location = 2;
    vertexAttributes[2].offset = offsetof(Vertex, texcoord);

    vertexInputState.vertex_attributes = vertexAttributes.data();
    vertexInputState.num_vertex_attributes = vertexAttributes.size();

    pipelinecreateinfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelinecreateinfo.vertex_shader = vertexShader.Get();
    pipelinecreateinfo.fragment_shader = fragmentShader.Get();

    m_textpipeline = SDL::GraphicsPipeline(m_gpu, pipelinecreateinfo);
}