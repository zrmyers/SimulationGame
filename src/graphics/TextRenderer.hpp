#pragma once

#include "Renderable.hpp"
#include "core/AssetLoader.hpp"
#include "sdl/TTF.hpp"
#include <glm/ext/matrix_float4x4.hpp>

#include "graphics/Renderer.hpp"

namespace Graphics {


    //! Everything necessary for rendering text.
    struct TextComponent {
        SDL::TTF::Text text;
        glm::mat4 world_transform;
        SDL::TTF::Font* p_font;
    };

    class TextRenderer {

        public:
            TextRenderer(Renderer& renderer, Core::AssetLoader& assetLoader);

            //! Create a font object. This is used for creating text.
            SDL::TTF::Font CreateFont(const std::string& filename, float ptsize);

            //! Create a text component.
            TextComponent CreateText(
                SDL::TTF::Font& font,
                const std::string& message);

            //! build a render list.
            std::vector<Renderable> BuildRenderables(std::list<TextComponent*>& textComponents);

        private:

            // Build a pipeline for rendering 3D text.
            void Build3DTextPipeline(Graphics::Renderer& renderer, const std::string& shader_path);

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
            Graphics::GeometryData m_geometry_data;

            //! TTF Context.
            SDL::TTF::Context m_ttf;

            //! Text Engine
            SDL::TTF::TextEngine m_textengine;
    };
}