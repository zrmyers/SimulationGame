#include "TextSystem.hpp"
#include "RenderSystem.hpp"
#include "core/AssetLoader.hpp"
#include "ecs/ECS.hpp"
#include "components/Renderable.hpp"
#include "components/Text.hpp"
#include "components/Transform.hpp"
#include "components/Camera.hpp"
#include "sdl/TTF.hpp"

#include <SDL3_ttf/SDL_ttf.h>
#include <array>
#include <glm/ext/matrix_transform.hpp>
#include <memory>

static const constexpr uint32_t MAX_VERTEX_COUNT = 4000;
static const constexpr uint32_t MAX_INDEX_COUNT = 6000;

Systems::TextSystem::TextSystem(Core::Engine& engine)
    : ECS::System(engine) {

    ECS::Registry& registry = engine.GetEcsRegistry();
    Systems::RenderSystem& renderSystem = registry.GetSystem<Systems::RenderSystem>();
    Core::AssetLoader& assetLoader = engine.GetAssetLoader();

    Build3DTextPipeline(renderSystem, assetLoader.GetShaderDir());

    m_vertex_buffer = renderSystem.CreateBuffer(
        SDL_GPU_BUFFERUSAGE_VERTEX,
        sizeof(Systems::Vertex) * MAX_VERTEX_COUNT);

    m_index_buffer = renderSystem.CreateBuffer(
        SDL_GPU_BUFFERUSAGE_INDEX,
        sizeof(int) * MAX_INDEX_COUNT);

    m_transferbuffer = renderSystem.CreateTransferBuffer(
        SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        (sizeof(Systems::Vertex) * MAX_VERTEX_COUNT) + (sizeof(int) * MAX_INDEX_COUNT));

    SDL_GPUSamplerCreateInfo sampler_info = {};
    sampler_info.min_filter = SDL_GPU_FILTER_LINEAR;
    sampler_info.mag_filter = SDL_GPU_FILTER_LINEAR,
    sampler_info.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sampler_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

    m_sampler = renderSystem.CreateSampler(sampler_info);

    m_geometry_data.vertices.resize(MAX_VERTEX_COUNT);
    m_geometry_data.indices.resize(MAX_INDEX_COUNT);
    m_textengine = SDL::TTF::TextEngine(renderSystem.GetGpuDevice());

    m_renderable = ECS::Entity(registry);
}

Systems::TextSystem::~TextSystem() {
}

std::shared_ptr<SDL::TTF::Font> Systems::TextSystem::CreateFont(const std::string& filename, float ptsize) { // NOLINT

    return std::make_shared<SDL::TTF::Font>(filename, ptsize);
}

void Systems::TextSystem::Update() {

    ECS::Registry& registry = GetEngine().GetEcsRegistry();
    std::set<ECS::EntityID_t> entities = GetEntities();
    const auto* cameras = registry.GetComponentArray<Components::Camera>();

    if (!entities.empty() && (cameras->GetSize() > 0)) {
        const Components::Camera& camera = cameras->GetByIndex(0);

        std::vector<Components::Renderable> renderlist;
        renderlist.reserve(entities.size());

        UpdateGeometryBuffer();
        SetupTransferBuffer();

        // Build renderable object
        Components::Renderable& renderable = m_renderable.FindOrEmplaceComponent<Components::Renderable>();

        // setup transfer
        renderable.is_loaded = false;
        renderable.m_vertex_src.transfer_buffer = m_transferbuffer.Get();
        renderable.m_vertex_src.offset = 0;
        renderable.m_vertex_dst.buffer = m_vertex_buffer.Get();
        renderable.m_vertex_dst.offset = 0;
        renderable.m_vertex_dst.size = sizeof(Systems::Vertex) * m_geometry_data.vertexCount;
        renderable.m_index_src.transfer_buffer = m_transferbuffer.Get();
        renderable.m_index_src.offset = sizeof(Systems::Vertex) * MAX_VERTEX_COUNT;
        renderable.m_index_dst.buffer = m_index_buffer.Get();
        renderable.m_index_dst.offset = 0;
        renderable.m_index_dst.size = sizeof(int) * m_geometry_data.indexCount;

        // resource bindings
        renderable.is_visible = true;
        renderable.m_vertex_buffer_binding.buffer = m_vertex_buffer.Get();
        renderable.m_vertex_buffer_binding.offset = 0;
        renderable.m_index_buffer_binding.buffer = m_index_buffer.Get();
        renderable.m_index_buffer_binding.offset = 0;
        renderable.m_p_pipeline = &m_textpipeline;

        renderable.m_drawcommands.clear();
        // batch draw commands
        int32_t indexCount = 0U;
        int32_t vertexOffset = 0U;
        for (ECS::EntityID_t entity : entities) {

            Components::Text& text = registry.GetComponent<Components::Text>(entity);
            Components::Transform& transform = registry.GetComponent<Components::Transform>(entity);

            renderable.uniform_data.projview = camera.GetProjection();
            renderable.uniform_data.model = transform.m_transform;
            for (TTF_GPUAtlasDrawSequence* p_current = text.m_p_text->GetGpuDrawData(); p_current != nullptr; p_current = p_current->next) {

                Components::DrawCommand& command = renderable.m_drawcommands.emplace_back();
                command.textureSampler = {p_current->atlas_texture, m_sampler.Get()};
                command.m_num_indices = p_current->num_indices;
                command.m_num_instances = 1;
                command.m_start_index = indexCount;
                command.m_vertex_offset = vertexOffset;
                command.m_start_instance = 0;

                indexCount += p_current->num_indices;
                vertexOffset += p_current->num_vertices;
            }
        }
    }
}

void Systems::TextSystem::UpdateGeometryBuffer() {

    ECS::Registry& registry = GetEngine().GetEcsRegistry();
    std::set<ECS::EntityID_t>& entities = GetEntities();
    m_geometry_data.vertexCount = 0;
    m_geometry_data.indexCount = 0;

    // Build renderable object for text buffer.
    for (ECS::EntityID_t entity : entities) {

        Components::Text& text = registry.GetComponent<Components::Text>(entity);

        if (text.m_p_font == nullptr) {
            continue;
        }

        if (text.m_p_text == nullptr) {

            // create new text.
            text.m_p_text = std::make_shared<SDL::TTF::Text>(m_textengine, *text.m_p_font, text.m_string);
        }

        text.m_p_text->SetString(text.m_string);

        // update geometry buffer.
        TTF_GPUAtlasDrawSequence* p_sequence = text.m_p_text->GetGpuDrawData();
        TTF_GPUAtlasDrawSequence* p_current = p_sequence;
        while (p_current != nullptr) {

            for (int i = 0; i < p_current->num_vertices; i++) {
                Systems::Vertex vert = {};
                SDL_FPoint& pos = p_current->xy[i]; // NOLINT
                SDL_FPoint& uv = p_current->uv[i]; // NOLINT
                vert.position = glm::vec3(pos.x, pos.y, 0.0F);
                vert.color = text.m_color;
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
    }
}

void Systems::TextSystem::SetupTransferBuffer() {

    // copy the geometry data to the transfer buffer.
    Systems::Vertex* p_data = reinterpret_cast<Systems::Vertex*>(m_transferbuffer.Map()); // NOLINT

    memcpy(p_data, m_geometry_data.vertices.data(), sizeof(Systems::Vertex)*m_geometry_data.vertexCount);
    memcpy(&(p_data[MAX_VERTEX_COUNT]), m_geometry_data.indices.data(), sizeof(int) * m_geometry_data.indexCount); // NOLINT

    m_transferbuffer.Unmap();
}

// Build a pipeline for rendering 3D text.
void Systems::TextSystem::Build3DTextPipeline(Systems::RenderSystem& renderer, const std::string& shader_path) {
    Graphics::ShaderCross compiler;

    // Load file from filesystem
    Graphics::ByteCode vertexShaderBytecode = compiler.CompileToSpirv(
        EShLangVertex, shader_path + "/guiText.vert.glsl", "main", shader_path, {});

    SDL::Shader vertexShader = renderer.CreateShader(vertexShaderBytecode);

    Graphics::ByteCode fragmentShaderBytecode = compiler.CompileToSpirv(
        EShLangFragment, shader_path + "/guiText.frag.glsl", "main", shader_path, {});

    SDL::Shader fragmentShader = renderer.CreateShader(fragmentShaderBytecode);

    SDL_GPUGraphicsPipelineCreateInfo pipelinecreateinfo = {};

    //------------------------------------------------------------------------------------------------------------------
    // Target Info Description

    SDL_GPUGraphicsPipelineTargetInfo& targetInfo = pipelinecreateinfo.target_info;

    SDL_GPUColorTargetDescription colorTargetDescription;
    colorTargetDescription.format = renderer.GetSwapchainTextureFormat();
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
    vertexBufferDescription.pitch = sizeof(Systems::Vertex);

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
    vertexAttributes[1].offset = offsetof(Systems::Vertex, color);
    vertexAttributes[2].buffer_slot = 0;
    vertexAttributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    vertexAttributes[2].location = 2;
    vertexAttributes[2].offset = offsetof(Systems::Vertex, texcoord);

    vertexInputState.vertex_attributes = vertexAttributes.data();
    vertexInputState.num_vertex_attributes = vertexAttributes.size();

    pipelinecreateinfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelinecreateinfo.vertex_shader = vertexShader.Get();
    pipelinecreateinfo.fragment_shader = fragmentShader.Get();

    m_textpipeline = renderer.CreatePipeline(pipelinecreateinfo);
}