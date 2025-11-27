#include "TextSystem.hpp"
#include "RenderSystem.hpp"
#include "core/AssetLoader.hpp"
#include "ecs/ECS.hpp"
#include "components/Renderable.hpp"
#include "components/Text.hpp"
#include "components/Transform.hpp"
#include "components/Camera.hpp"
#include "graphics/pipelines/UnlitTexturePipeline.hpp"
#include "sdl/SDL.hpp"
#include "sdl/TTF.hpp"

#include <SDL3/SDL_gpu.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdint>
#include <glm/ext/matrix_transform.hpp>
#include <memory>

static const constexpr uint32_t MAX_VERTEX_COUNT = 4000;
static const constexpr uint32_t MAX_INDEX_COUNT = 6000;

Systems::TextSystem::TextSystem(Core::Engine& engine)
    : ECS::System(engine) {

    ECS::Registry& registry = engine.GetEcsRegistry();
    Systems::RenderSystem& renderSystem = registry.GetSystem<Systems::RenderSystem>();
    Core::AssetLoader& assetLoader = engine.GetAssetLoader();

    m_p_textpipeline = renderSystem.CreatePipeline<Graphics::UnlitTexturePipeline>();
    m_p_textpipeline_sdf = renderSystem.CreatePipeline<Graphics::UnlitTextureSDFPipeline>();

    m_vertex_buffer = renderSystem.CreateBuffer(
        SDL_GPU_BUFFERUSAGE_VERTEX,
        sizeof(Graphics::UnlitTexturedVertex) * MAX_VERTEX_COUNT);
    m_vertex_buffer.SetBufferName("Text Vertex Buffer");
    m_index_buffer = renderSystem.CreateBuffer(
        SDL_GPU_BUFFERUSAGE_INDEX,
        sizeof(int) * MAX_INDEX_COUNT);
    m_index_buffer.SetBufferName("Text Index Buffer");

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

std::shared_ptr<SDL::TTF::Font> Systems::TextSystem::CreateFont(const std::string& filename, float ptsize) { // NOLINT

    return std::make_shared<SDL::TTF::Font>(filename, ptsize);
}

void Systems::TextSystem::Update() {

    ECS::Registry& registry = GetEngine().GetEcsRegistry();
    Systems::RenderSystem& rendersystem = registry.GetSystem<Systems::RenderSystem>();
    std::set<ECS::EntityID_t> entities = GetEntities();
    const auto* cameras = registry.GetComponentArray<Components::Camera>();

    if (!entities.empty() && (cameras->GetSize() > 0)) {
        const Components::Camera& camera = cameras->GetByIndex(0);

        std::vector<Components::Renderable> renderlist;
        renderlist.reserve(entities.size());

        UpdateGeometryBuffer();

        // Build renderable object
        Components::Renderable& renderable = m_renderable.FindOrEmplaceComponent<Components::Renderable>();

        // resource bindings
        renderable.is_visible = true;
        renderable.m_vertex_buffer_binding.buffer = m_vertex_buffer.Get();
        renderable.m_vertex_buffer_binding.offset = 0;
        renderable.m_index_buffer_binding.buffer = m_index_buffer.Get();
        renderable.m_index_buffer_binding.offset = 0;
        renderable.m_index_size = SDL_GPU_INDEXELEMENTSIZE_32BIT;
        renderable.m_p_pipeline = m_p_textpipeline;
        renderable.m_requests = SetupTransferBuffer(rendersystem);
        renderable.m_drawcommands.clear();
        // batch draw commands
        int32_t indexCount = 0U;
        int32_t vertexOffset = 0U;
        for (ECS::EntityID_t entity : entities) {

            Components::Text& text = registry.GetComponent<Components::Text>(entity);
            Components::Transform& transform = registry.GetComponent<Components::Transform>(entity);

            if (text.m_p_font->GetSDF()) {
                renderable.m_p_pipeline = m_p_textpipeline_sdf;
            }
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
                Graphics::UnlitTexturedVertex vert = {};
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

std::vector<Systems::RenderSystem::TransferRequest> Systems::TextSystem::SetupTransferBuffer(Systems::RenderSystem& renderSystem) {

    // copy the geometry data to the transfer buffer.
    std::vector<Systems::RenderSystem::TransferRequest> requests;

    // vertex data
    {
        Systems::RenderSystem::TransferRequest request = {};
        request.cycle = true; // don't overwrite vertex data in previous frame.
        request.type = RenderSystem::RequestType::UPLOAD_TO_BUFFER;
        request.data.buffer.buffer = m_vertex_buffer.Get();
        request.data.buffer.offset = 0;
        request.data.buffer.size = sizeof(Graphics::UnlitTexturedVertex) * m_geometry_data.vertexCount;
        request.p_src = m_geometry_data.vertices.data();
        requests.push_back(request);
    }

    // index data
    {
        Systems::RenderSystem::TransferRequest request = {};
        request.cycle = true; // don't overwrite vertex data in previous frame.
        request.type = RenderSystem::RequestType::UPLOAD_TO_BUFFER;
        request.data.buffer.buffer = m_index_buffer.Get();
        request.data.buffer.offset = 0;
        request.data.buffer.size = sizeof(uint32_t) * m_geometry_data.indexCount;
        request.p_src = m_geometry_data.indices.data();
        requests.push_back(request);
    }

    return requests;
}
