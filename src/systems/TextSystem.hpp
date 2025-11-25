#pragma once

#include "RenderSystem.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "sdl/TTF.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <unordered_map>

namespace Systems {

    class RenderSystem;

    struct Vertex {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texcoord;
    };

    struct GeometryData {
        std::vector<Vertex> vertices;
        int vertexCount = 0;
        std::vector<int> indices;
        int indexCount = 0;
    };

    class TextSystem : public ECS::System {

        public:
            TextSystem(Core::Engine& engine);

            ~TextSystem() override;

            //! Create a font object. This is used for creating text.
            std::shared_ptr<SDL::TTF::Font> CreateFont(const std::string& filename, float ptsize);

            void Update() override;

            //! build a render list.
            //std::vector<Components::Renderable> BuildRenderables(std::list<TextComponent*>& textComponents);

        private:

            // Build a pipeline for rendering 3D text.
            void Build3DTextPipeline(Systems::RenderSystem& renderer, const std::string& shader_path);

            // Update geometry buffer
            void UpdateGeometryBuffer();

            // Upload geometry to GPU
            void SetupTransferBuffer();

            // Update renderables
            void UpdateRenderable();

            // Text pipeline
            SDL::GraphicsPipeline m_textpipeline;

            //! Text buffers
            SDL::GpuBuffer m_vertex_buffer;
            SDL::GpuBuffer m_index_buffer;

            //! Transfer Buffer. Use for uploading data to GPU.
            SDL::GpuTransferBuffer m_transferbuffer;

            //! Texture sampler.
            SDL::GpuSampler m_sampler;

            //! Geometry data
            Systems::GeometryData m_geometry_data;

            //! TTF Context.
            SDL::TTF::Context m_ttf;

            //! Text Engine
            SDL::TTF::TextEngine m_textengine;

            //! List of render objects.
            ECS::Entity m_renderable;

    };
}