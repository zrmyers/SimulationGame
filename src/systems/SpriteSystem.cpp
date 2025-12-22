#include "SpriteSystem.hpp"
#include "RenderSystem.hpp"
#include "components/Renderable.hpp"
#include "components/Sprite.hpp"
#include "components/Transform.hpp"
#include "core/AssetLoader.hpp"
#include "core/Engine.hpp"
#include "core/Logger.hpp"
#include "ecs/ECS.hpp"
#include "graphics/Mesh.hpp"
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

    m_p_sprite_mesh = std::make_unique<Graphics::Mesh>(renderSys.CreateMesh(
        sizeof(Graphics::UnlitTexturedVertex),
        MAX_VERTEX_COUNT,
        SDL_GPU_INDEXELEMENTSIZE_16BIT,
        MAX_INDEX_COUNT));

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

            renderable.m_p_mesh = m_p_sprite_mesh.get();
            renderable.m_p_pipeline = m_p_sprite_pipeline;
            renderable.transform = transform.m_transform;
            renderable.textureSampler = sprite.texture->GetBinding();

            Components::DrawCommand& command = renderable.m_drawcommand;
            command.m_num_indices = 6;
            command.m_num_instances = 1U;
            command.m_start_index = startIndex;
            command.m_vertex_offset = 0;//static_cast<int32_t>(numVertices);
            command.m_start_instance = 0U;

            renderable.m_layer = sprite.layer;
            renderable.m_depth_override = sprite.draw_order;
        }

        m_p_sprite_mesh->LoadData(m_vertices, m_indices);
    }
}
