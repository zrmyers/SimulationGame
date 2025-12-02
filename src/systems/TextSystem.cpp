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
#include <algorithm>
#include <cstdint>
#include <glm/ext/matrix_transform.hpp>
#include <memory>

static const constexpr uint32_t MAX_VERTEX_COUNT = 4000;
static const constexpr uint32_t MAX_INDEX_COUNT = 6000;

// conversion of point size to pixel size
static const constexpr float POINT_TO_PIXEL = 2.0F;

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

    m_geometry_data.vertices.reserve(MAX_VERTEX_COUNT);
    m_geometry_data.indices.reserve(MAX_INDEX_COUNT);
    m_textengine = SDL::TTF::TextEngine(renderSystem.GetGpuDevice());
}

std::shared_ptr<SDL::TTF::Font> Systems::TextSystem::CreateFont(const std::string& filename, float ptsize) { // NOLINT

    return std::make_shared<SDL::TTF::Font>(filename, ptsize);
}

std::shared_ptr<SDL::TTF::Text> Systems::TextSystem::CreateText(std::shared_ptr<SDL::TTF::Font>& p_font, const std::string& text) { // NOLINT
    return std::make_shared<SDL::TTF::Text>(m_textengine, *p_font, text);
}

void Systems::TextSystem::Update() {

    ECS::Registry& registry = GetEngine().GetEcsRegistry();
    Systems::RenderSystem& rendersystem = registry.GetSystem<Systems::RenderSystem>();
    std::set<ECS::EntityID_t> entities = GetEntities();

    if (!entities.empty()) {

        // update buffers
        UpdateGeometryBuffer();

        // upload data to gpu
        rendersystem.UploadDataToBuffer(SetupTransferBuffer(rendersystem));

        uint32_t indexCount = 0U;
        // Build renderable object for each entity
        for (ECS::EntityID_t entity : entities) {

            Components::Text& text = registry.GetComponent<Components::Text>(entity);
            Components::Transform& transform = registry.GetComponent<Components::Transform>(entity);

            if ((text.m_p_font == nullptr) || (text.m_p_text == nullptr)) {
                continue;
            }

            Components::Renderable& renderable = registry.FindOrEmplaceComponent<Components::Renderable>(entity);

            if (text.m_p_font->GetSDF()) {
                renderable.m_p_pipeline = m_p_textpipeline_sdf;
            }
            else {
                renderable.m_p_pipeline = m_p_textpipeline;
            }
            renderable.m_layer = text.m_layer;
            renderable.transform = transform.m_transform;

            renderable.m_vertex_buffer_binding.buffer = m_vertex_buffer.Get();
            renderable.m_vertex_buffer_binding.offset = 0;
            renderable.m_index_buffer_binding.buffer = m_index_buffer.Get();
            renderable.m_index_buffer_binding.offset = 0;
            renderable.textureSampler.sampler = m_sampler.Get();

            renderable.m_drawcommand.m_start_index = indexCount;
            renderable.m_drawcommand.m_vertex_offset = 0;
            renderable.m_drawcommand.m_start_instance = 0;
            renderable.m_drawcommand.m_num_instances = 1;
            renderable.m_index_size = SDL_GPU_INDEXELEMENTSIZE_32BIT;

            uint32_t indicesInTextObject = 0U;
            for (TTF_GPUAtlasDrawSequence* p_current = text.m_p_text->GetGpuDrawData(); p_current != nullptr; p_current = p_current->next) {

                if (renderable.textureSampler.texture != p_current->atlas_texture) {
                    renderable.textureSampler.texture = p_current->atlas_texture;
                }

                indicesInTextObject += p_current->num_indices;
            }
            renderable.m_drawcommand.m_num_indices = indicesInTextObject;
            indexCount += indicesInTextObject;

        }
    }

}

void Systems::TextSystem::GetTextMetrics(const TTF_GPUAtlasDrawSequence* p_draw_sequence, glm::vec2& min, glm::vec2& max) const {

    float xmin = p_draw_sequence->xy[0].x;
    float xmax = p_draw_sequence->xy[0].x;
    float ymin = p_draw_sequence->xy[0].y;
    float ymax = p_draw_sequence->xy[0].y;

    const TTF_GPUAtlasDrawSequence* p_current = p_draw_sequence;
    while (p_current != nullptr) {

        int32_t indexStart = static_cast<int32_t>(m_geometry_data.vertices.size());
        for (int i = 0; i < p_current->num_vertices; i++) {
            SDL_FPoint& pos = p_current->xy[i]; // NOLINT

            xmax = std::max(pos.x, xmax);
            xmin = std::min(pos.x, xmin);
            ymax = std::max(pos.y, ymax);
            ymin = std::max(pos.y, ymin);
        }

        p_current = p_current->next;
    }

    min = {xmin, ymin}; // bottom left
    max = {xmax, ymax}; // top right
}

void Systems::TextSystem::UpdateGeometryBuffer() {

    ECS::Registry& registry = GetEngine().GetEcsRegistry();
    std::set<ECS::EntityID_t>& entities = GetEntities();
    m_geometry_data.vertices.clear();
    m_geometry_data.indices.clear();

    // Build renderable object for text buffer.
    for (ECS::EntityID_t entity : entities) {

        Components::Text& text = registry.GetComponent<Components::Text>(entity);

        if ((text.m_p_font == nullptr) || (text.m_p_text == nullptr)) {
            continue;
        }

        // update geometry buffer.
        TTF_GPUAtlasDrawSequence* p_sequence = text.m_p_text->GetGpuDrawData();
        TTF_GPUAtlasDrawSequence* p_current = p_sequence;

        while (p_current != nullptr) {

            int32_t indexStart = static_cast<int32_t>(m_geometry_data.vertices.size());
            for (int i = 0; i < p_current->num_vertices; i++) {
                Graphics::UnlitTexturedVertex vert = {};
                SDL_FPoint& pos = p_current->xy[i]; // NOLINT
                SDL_FPoint& uv = p_current->uv[i]; // NOLINT

                vert.position = glm::vec3(pos.x, pos.y, 0.0F) * POINT_TO_PIXEL;
                vert.color = text.m_color;
                vert.texcoord = glm::vec2(uv.x, uv.y);

                m_geometry_data.vertices.push_back(vert);
            }

            for (int i = 0; i < p_current->num_indices; i++) {
                m_geometry_data.indices.push_back(indexStart + p_current->indices[i]);
            }

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
        request.data.buffer.size = sizeof(Graphics::UnlitTexturedVertex) * m_geometry_data.vertices.size();
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
        request.data.buffer.size = sizeof(uint32_t) * m_geometry_data.indices.size();
        request.p_src = m_geometry_data.indices.data();
        requests.push_back(request);
    }

    return requests;
}
