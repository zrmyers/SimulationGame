#include "Renderer.hpp"

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

    m_font = SDL::TTF::Font(asset_loader.GetFontDir() + "/Oblegg-Regular.otf", 50.0F); // NOLINT

    m_font.SetHorizontalAlignment(TTF_HORIZONTAL_ALIGN_CENTER);

    m_textengine = SDL::TTF::TextEngine(m_gpu);

    m_text = SDL::TTF::Text(m_textengine, m_font, "Hello Text!");

    m_uniform.projview = glm::perspective(glm::pi<float>() / 2.0f, 1024.0F / 768.0F, 0.1F, 100.0F);
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

        int textWidth = 0;
        int textHeight = 0;
        m_text.GetSize(textWidth, textHeight);
        // text pipeline
        m_uniform.model = glm::mat4(1.0F);
        m_uniform.model = glm::translate(m_uniform.model, glm::vec3(0.0F, 0.0F, -80.0F)); // NOLINT
        m_uniform.model = glm::scale(m_uniform.model, glm::vec3(0.3F, 0.3F, 0.3F)); // NOLINT
        m_uniform.model = glm::translate(m_uniform.model,
            glm::vec3(static_cast<float>(-textWidth) / 2.0F, static_cast<float>(textHeight) / 2.0F, 0.0F)); // NOLINT

        // Update Text Geometry Buffer.
        TTF_GPUAtlasDrawSequence* p_sequence = m_text.GetGpuDrawData();
        TTF_GPUAtlasDrawSequence* p_current = p_sequence;
        while (p_current != nullptr) {

            for (int i = 0; i < p_current->num_vertices; i++) {
                Vertex vert = {};
                SDL_FPoint& pos = p_current->xy[i]; // NOLINT
                SDL_FPoint& uv = p_current->uv[i]; // NOLINT
                vert.position = glm::vec3(pos.x, pos.y, 0.0F);
                vert.color = glm::vec4(0.0F, 1.0F, 0.0F, 1.0F);
                vert.texcoord = glm::vec2(uv.x, uv.y);

                m_geometry_data.vertices[m_geometry_data.vertexCount + i] = vert;
            }

            memcpy(
                &m_geometry_data.indices.at(m_geometry_data.indexCount),
                p_current->indices,
                p_current->num_indices * sizeof(int));
            m_geometry_data.vertexCount += p_current->num_vertices;
            m_geometry_data.indexCount += p_current->num_indices;

            p_current = p_current->next;
        }

        // copy the geometry data to the transfer buffer.
        Vertex* p_data = reinterpret_cast<Vertex*>(m_transferbuffer.Map()); // NOLINT

        memcpy(p_data, m_geometry_data.vertices.data(), sizeof(Vertex)*m_geometry_data.vertexCount);
        memcpy(&(p_data[MAX_VERTEX_COUNT]), m_geometry_data.indices.data(), sizeof(int) * m_geometry_data.indexCount); // NOLINT

        m_transferbuffer.Unmap();

        // upload to the GPU.
        SDL_GPUCopyPass* p_copyPass = SDL_BeginGPUCopyPass(p_cmdbuf);
        SDL_GPUTransferBufferLocation source = {};
        source.transfer_buffer = m_transferbuffer.Get();
        source.offset = 0;
        SDL_GPUBufferRegion destination = {};
        destination.buffer = m_vertexbuffer.Get();
        destination.offset = 0;
        destination.size = sizeof(Vertex) * m_geometry_data.vertexCount;

        SDL_UploadToGPUBuffer(p_copyPass, &source, &destination, false);
        source.offset = sizeof(Vertex) * MAX_VERTEX_COUNT;
        destination.buffer = m_indexbuffer.Get();
        destination.offset = 0;
        destination.size = sizeof(int) * m_geometry_data.indexCount;
        SDL_UploadToGPUBuffer(p_copyPass, &source, &destination, false);
        SDL_EndGPUCopyPass(p_copyPass);

        // Draw commands
        SDL_GPURenderPass* p_renderPass = SDL_BeginGPURenderPass(p_cmdbuf, &colorTargetInfo, 1U, nullptr);

        // simple pipeline
        SDL_BindGPUGraphicsPipeline(p_renderPass, m_fillpipeline.Get());
        SDL_DrawGPUPrimitives(p_renderPass, 3, 1, 0, 0);

        // text pipeline
        SDL_BindGPUGraphicsPipeline(p_renderPass, m_textpipeline.Get());
        SDL_GPUBufferBinding binding = {};
        binding.buffer = m_vertexbuffer.Get();
        binding.offset = 0;
        SDL_BindGPUVertexBuffers(p_renderPass, 0, &binding, 1U);
        binding.buffer = m_indexbuffer.Get();
        binding.offset = 0;
        SDL_BindGPUIndexBuffer(p_renderPass, &binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
        SDL_PushGPUVertexUniformData(p_cmdbuf, 0U, &m_uniform, sizeof(m_uniform));

        int indexOffset = 0;
        int vertexOffset = 0;
        for (; p_sequence != nullptr; p_sequence = p_sequence->next) {
            SDL_GPUTextureSamplerBinding samplerBinding = {};
            samplerBinding.sampler = m_sampler.Get();
            samplerBinding.texture = p_sequence->atlas_texture;
            SDL_BindGPUFragmentSamplers(p_renderPass, 0U, &samplerBinding, 1U);

            SDL_DrawGPUIndexedPrimitives(p_renderPass, p_sequence->num_indices, 1, indexOffset, vertexOffset, 0);

            indexOffset += p_sequence->num_indices;
            vertexOffset += p_sequence->num_vertices;
        }

        SDL_EndGPURenderPass(p_renderPass);
    }

    SDL_SubmitGPUCommandBuffer(p_cmdbuf);

    m_geometry_data.indexCount = 0;
    m_geometry_data.vertexCount = 0;
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