#pragma once

#include "RenderSystem.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "graphics/pipelines/PipelineCache.hpp"
#include "graphics/pipelines/UnlitTexturePipeline.hpp"
#include "sdl/SDL.hpp"
#include "sdl/TTF.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <memory>

namespace Systems {

    class RenderSystem;

    struct GeometryData {
        std::vector<Graphics::UnlitTexturedVertex> vertices;
        std::vector<int> indices;
    };

    class TextSystem : public ECS::System {

        public:
            TextSystem(Core::Engine& engine);

            //! Create a font object. This is used for creating text.
            std::shared_ptr<SDL::TTF::Font> CreateFont(const std::string& filename, float ptsize);

            void Update() override;

        private:

            // Update geometry buffer
            void UpdateGeometryBuffer();

            // Upload geometry to GPU
            std::vector<Systems::RenderSystem::TransferRequest> SetupTransferBuffer(Systems::RenderSystem& renderSystem);

            // Update renderables
            void UpdateRenderable();

            // Normal Text pipeline
            Graphics::IPipeline* m_p_textpipeline;

            // SDF Text pipeline. Used for rendering fonts that use signed-distance fields.
            Graphics::IPipeline* m_p_textpipeline_sdf;

            //! Text buffers
            SDL::GpuBuffer m_vertex_buffer;
            SDL::GpuBuffer m_index_buffer;

            //! Texture sampler.
            SDL::GpuSampler m_sampler;

            //! Geometry data
            Systems::GeometryData m_geometry_data;

            //! TTF Context.
            SDL::TTF::Context m_ttf;

            //! Text Engine
            SDL::TTF::TextEngine m_textengine;
    };
}