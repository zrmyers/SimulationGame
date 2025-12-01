#include "SpriteSystem.hpp"
#include "RenderSystem.hpp"
#include "components/Renderable.hpp"
#include "components/Sprite.hpp"
#include "components/Transform.hpp"
#include "core/AssetLoader.hpp"
#include "core/Engine.hpp"
#include "core/Logger.hpp"
#include "ecs/ECS.hpp"
#include "graphics/ShaderCross.hpp"
#include "graphics/pipelines/UnlitTexturePipeline.hpp"
#include "sdl/SDL.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <cstddef>
#include <cstdint>
#include <glslang/Public/ShaderLang.h>
#include <string>

// 100 KB should be enough
static constexpr uint32_t MAX_SPRITE_COUNT = 1000; // seems like enough for now. when render graph is implemented, this can become dynamic.
static constexpr uint32_t MAX_VERTEX_COUNT = MAX_SPRITE_COUNT * 4;
static constexpr uint32_t MAX_INDEX_COUNT = MAX_SPRITE_COUNT * 6;

Systems::SpriteSystem::SpriteSystem(Core::Engine& engine)
    : ECS::System(engine) {

    Core::AssetLoader& assetLoader = engine.GetAssetLoader();
    Systems::RenderSystem& renderSys = engine.GetEcsRegistry().GetSystem<Systems::RenderSystem>();

    m_vertices.reserve(MAX_VERTEX_COUNT);
    m_indices.reserve(MAX_INDEX_COUNT);

    m_p_sprite_pipeline = renderSys.CreatePipeline<Graphics::UnlitTexturePipeline>();

    m_vertex_buffer = renderSys.CreateBuffer(SDL_GPU_BUFFERUSAGE_VERTEX, MAX_VERTEX_COUNT * sizeof(Graphics::UnlitTexturedVertex));
    m_vertex_buffer.SetBufferName("Sprite Vertex Buffer");
    m_index_buffer = renderSys.CreateBuffer(SDL_GPU_BUFFERUSAGE_INDEX, MAX_INDEX_COUNT * sizeof(uint16_t));
    m_index_buffer.SetBufferName("Sprite Index Buffer");

}

void Systems::SpriteSystem::Update() {

    ECS::Registry& registry = GetEngine().GetEcsRegistry();
    std::set<ECS::EntityID_t>& entities = GetEntities();

    if (!entities.empty()) {
        m_vertices.clear();
        m_indices.clear();

        // sort sprites into batches
        for (ECS::EntityID_t entityID : GetEntities()) {

            // inputs
            auto& sprite = registry.GetComponent<Components::Sprite>(entityID);
            auto& transform = registry.GetComponent<Components::Transform>(entityID);

            // output
            auto& renderable = registry.FindOrEmplaceComponent<Components::Renderable>(entityID);

            size_t startIndex = m_indices.size();
            size_t numVertices = m_vertices.size();
            // buffer vertex transfer
            m_vertices.push_back({{-1.0F, 1.0F, 0.0F}, sprite.color, sprite.topLeftUV});
            m_vertices.push_back({{1.0F, 1.0F, 0.0F}, sprite.color, {sprite.bottomRightUV.x, sprite.topLeftUV.y}});
            m_vertices.push_back({{1.0F, -1.0F, 0.0F}, sprite.color, sprite.bottomRightUV});
            m_vertices.push_back({{-1.0F, -1.0F, 0.0F}, sprite.color, {sprite.topLeftUV.x, sprite.bottomRightUV.y}});

            m_indices.push_back(numVertices + 0);
            m_indices.push_back(numVertices + 1);
            m_indices.push_back(numVertices + 2);
            m_indices.push_back(numVertices + 0);
            m_indices.push_back(numVertices + 2);
            m_indices.push_back(numVertices + 3);

            renderable.m_vertex_buffer_binding.buffer = m_vertex_buffer.Get();
            renderable.m_vertex_buffer_binding.offset = 0U;
            renderable.m_index_buffer_binding.buffer = m_index_buffer.Get();
            renderable.m_index_buffer_binding.offset = 0U;
            renderable.m_index_size = SDL_GPU_INDEXELEMENTSIZE_16BIT;
            renderable.m_p_pipeline = m_p_sprite_pipeline;
            renderable.transform = transform.m_transform;
            renderable.textureSampler.sampler = sprite.sampler->Get();
            renderable.textureSampler.texture = sprite.texture->Get();

            Components::DrawCommand& command = renderable.m_drawcommand;
            command.m_num_indices = 6;
            command.m_num_instances = 1U;
            command.m_start_index = startIndex;
            command.m_vertex_offset = 0;//static_cast<int32_t>(numVertices);
            command.m_start_instance = 0U;
            renderable.m_layer = sprite.layer;
        }

        UploadData(registry.GetSystem<Systems::RenderSystem>());
    }
}

void Systems::SpriteSystem::UploadData(Systems::RenderSystem& rendersystem) {

    // setup transfer buffers
    std::vector<Systems::RenderSystem::TransferRequest> requests;
    requests.reserve(2);
    Systems::RenderSystem::TransferRequest request = {};

    request.cycle = false;
    request.type = Systems::RenderSystem::RequestType::UPLOAD_TO_BUFFER;
    SDL_GPUBufferRegion& vertexRegion = request.data.buffer;
    vertexRegion.buffer = m_vertex_buffer.Get();
    vertexRegion.offset = 0;
    vertexRegion.size = sizeof(Graphics::UnlitTexturedVertex) * m_vertices.size();
    request.p_src = m_vertices.data();
    requests.push_back(request);

    request.cycle = false;
    request.type = Systems::RenderSystem::RequestType::UPLOAD_TO_BUFFER;
    SDL_GPUBufferRegion& indexRegion = request.data.buffer;
    indexRegion.buffer = m_index_buffer.Get();
    indexRegion.offset = 0;
    indexRegion.size = sizeof(uint16_t) * m_indices.size();
    request.p_src = m_indices.data();
    requests.push_back(request);

    rendersystem.UploadDataToBuffer(requests);
}