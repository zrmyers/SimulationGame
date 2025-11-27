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

    SDL_GPUSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    samplerCreateInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerCreateInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.enable_anisotropy = true;
    samplerCreateInfo.max_anisotropy = 16; // NOLINT

    m_sampler = renderSys.CreateSampler(samplerCreateInfo);

    m_vertex_buffer = renderSys.CreateBuffer(SDL_GPU_BUFFERUSAGE_VERTEX, MAX_VERTEX_COUNT * sizeof(Graphics::UnlitTexturedVertex));
    m_vertex_buffer.SetBufferName("Sprite Vertex Buffer");
    m_index_buffer = renderSys.CreateBuffer(SDL_GPU_BUFFERUSAGE_INDEX, MAX_INDEX_COUNT * sizeof(uint16_t));
    m_index_buffer.SetBufferName("Sprite Index Buffer");

}

void Systems::SpriteSystem::Update() {

    std::unordered_map<SDL_GPUTexture*, ECS::Entity> renderBatches;
    // sort the sprites
    std::unordered_map<SDL_GPUTexture*, std::list<ECS::EntityID_t>> entitiesByTexture;
    ECS::Registry& registry = GetEngine().GetEcsRegistry();

    // sort sprites into batches
    for (ECS::EntityID_t entityID : GetEntities()) {

        auto& sprite = registry.GetComponent<Components::Sprite>(entityID);

        auto batchIter = entitiesByTexture.find(sprite.texture.Get());
        if (batchIter != entitiesByTexture.end()) {
            batchIter->second.push_back(entityID);
        }
        else {
            entitiesByTexture[sprite.texture.Get()] = {entityID};
        }
    }

    m_vertices.clear();
    m_indices.clear();

    uint32_t num_vertices = 0U;
    uint32_t vertex_offset = 0U;
    uint32_t num_indices = 0U;
    uint32_t index_offset = 0U;

    // for each batch, build sprite geometry, and output renderable.
    for (auto& batchIter : entitiesByTexture) {

        // update geometry buffers
        for (ECS::EntityID_t entityID : batchIter.second) {

            auto& sprite = registry.GetComponent<Components::Sprite>(entityID);
            auto& transform = registry.GetComponent<Components::Transform>(entityID);

            InputSprite(sprite, transform);
            num_vertices += 4;
            num_indices += 6;
        }

        // reuse existing entity from previous list. Otherwise create a new entity.
        auto renderableIter = m_sprite_batches.find(batchIter.first);
        if (renderableIter != m_sprite_batches.end()) {

            renderBatches.insert(m_sprite_batches.extract(batchIter.first));
        }
        else {
            renderBatches[batchIter.first] = ECS::Entity(registry);
        }

        ECS::Entity& entity = renderBatches.at(batchIter.first);
        Components::Renderable& renderable = entity.FindOrEmplaceComponent<Components::Renderable>();
        OutputBatch(
            renderable,
            batchIter.first,
            vertex_offset, // starting vertex in batch
            num_vertices, // number of vertices in batch
            index_offset, // starting index in batch
            num_indices); // number of vertices in batch
        vertex_offset += num_vertices;
        index_offset += num_indices;
        num_vertices = 0;
        num_indices = 0;
    }

    renderable.is_visible = true;
    renderable.m_vertex_buffer_binding.buffer = m_vertex_buffer.Get();
    renderable.m_vertex_buffer_binding.offset = 0;
    renderable.m_index_buffer_binding.buffer = m_index_buffer.Get();
    renderable.m_index_buffer_binding.offset = 0;
    renderable.m_index_size = SDL_GPU_INDEXELEMENTSIZE_16BIT;
    renderable.m_drawcommands.clear();
    renderable.m_p_pipeline = m_p_sprite_pipeline;

    renderable.uniform_data.model = glm::mat4(1.0F);
    renderable.uniform_data.projview = glm::mat4(1.0F);

    Components::DrawCommand& command = renderable.m_drawcommands.emplace_back();
    command.m_num_indices = 6;
    command.m_num_instances = 1;
    command.m_start_index = 0;
    command.m_start_instance = 0;
    command.m_vertex_offset = 0;
    command.textureSampler.sampler = m_sampler.Get();
    command.textureSampler.texture = m_texture.Get();
}

void Systems::SpriteSystem::UploadData(Systems::RenderSystem& renderSystem) {

    // setup transfer buffers
    std::vector<Systems::RenderSystem::TransferRequest> requests;
    requests.reserve(2);
    Systems::RenderSystem::TransferRequest request = {};


    request.cycle = false;
    request.type = Systems::RenderSystem::RequestType::UPLOAD_TO_BUFFER;
    SDL_GPUBufferRegion& vertexRegion = request.data.buffer;
    vertexRegion.buffer = m_vertex_buffer.Get();
    vertexRegion.offset = 0;
    vertexRegion.size = sizeof(Graphics::UnlitTexturedVertex) * vertices.size();
    request.p_src = vertices.data();
    requests.push_back(request);

    std::vector<uint16_t> indexData;
    indexData.resize(6);

    request.cycle = false;
    request.type = Systems::RenderSystem::RequestType::UPLOAD_TO_BUFFER;
    SDL_GPUBufferRegion& indexRegion = request.data.buffer;
    indexRegion.buffer = m_index_buffer.Get();
    indexRegion.offset = 0;
    indexRegion.size = sizeof(uint16_t) * indexData.size();
    request.p_src = indexData.data();
    requests.push_back(request);

    renderSystem.UploadDataToBuffer(requests);
}

void Systems::SpriteSystem::InputSprite(const Components::Sprite& sprite, const Components::Transform& transform) {

    size_t vertex_offset = m_vertices.size();
    // buffer vertex transfer
    m_vertices.push_back({{-1.0F, 1.0F, 0.0F}, sprite.color, sprite.topLeftUV});
    m_vertices.push_back({{1.0F, 1.0F, 0.0F}, sprite.color, {sprite.bottomRightUV.x, sprite.topLeftUV.y}});
    m_vertices.push_back({{1.0F, -1.0F, 0.0F}, sprite.color, sprite.bottomRightUV});
    m_vertices.push_back({{-1.0F, -1.0F, 0.0F}, sprite.color, {sprite.topLeftUV.x, sprite.bottomRightUV.y}});

    m_indices.push_back(vertex_offset + 0);
    m_indices.push_back(vertex_offset + 1);
    m_indices.push_back(vertex_offset + 2);
    m_indices.push_back(vertex_offset + 0);
    m_indices.push_back(vertex_offset + 2);
    m_indices.push_back(vertex_offset + 3);
}

void Systems::SpriteSystem::OutputBatch(
    Components::Renderable& renderable,
    SDL_GPUTexture* p_texture,
    uint32_t vertexOffset,
    uint32_t numVertices,
    uint32_t indexOffset,
    uint32_t numIndices) {

}