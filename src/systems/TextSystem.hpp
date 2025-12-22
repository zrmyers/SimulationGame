#pragma once

#include "RenderSystem.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/pipelines/PipelineCache.hpp"
#include "graphics/pipelines/UnlitTexturePipeline.hpp"
#include "graphics/Font.hpp"
#include "sdl/SDL.hpp"
#include "sdl/TTF.hpp"
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdint>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_uint2.hpp>
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
            std::shared_ptr<Graphics::Font> CreateFont(const std::string& filename, float ptsize, bool useSDF, TTF_HorizontalAlignment alignment);

            void Update() override;

            SDL::TTF::TextEngine& GetTextEngine();
        private:

            // Get Text Metrics
            void GetTextMetrics(const TTF_GPUAtlasDrawSequence* p_draw_sequence, glm::vec2& min, glm::vec2& max) const;

            // Update geometry buffer
            void UpdateGeometryBuffer();

            // Update renderables
            void UpdateRenderable();

            // Normal Text pipeline
            Graphics::IPipeline* m_p_textpipeline;

            // SDF Text pipeline. Used for rendering fonts that use signed-distance fields.
            Graphics::IPipeline* m_p_textpipeline_sdf;

            //! Text buffers
            std::unique_ptr<Graphics::Mesh> m_p_text_mesh;

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