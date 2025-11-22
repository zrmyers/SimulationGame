#pragma once

#include "core/AssetLoader.hpp"
#include "sdl/SDL.hpp"
#include <string>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

#include "sdl/TTF.hpp"

namespace Graphics {

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

    struct UniformData {
        glm::mat4 projview;
        glm::mat4 model;
    };

    class Renderer {

        public:

            //! Initialize the renderer.
            Renderer(SDL::Context& context, Core::AssetLoader& asset_loader);
            Renderer(const Renderer& other) = delete;
            Renderer(Renderer&& other) = delete;
            Renderer& operator=(const Renderer& other) = delete;
            Renderer& operator=(Renderer&& other) = delete;
            ~Renderer();

            //! Draws all visible objects.
            void Draw();

        private:

            // Build a simple pipeline.
            void BuildSimplePipeline(const std::string& shader_path);

            // Build a pipeline for rendering 3D text.
            void Build3DTextPipeline(const std::string& shader_path);

            //! Window used for rendering graphics to screen.
            SDL::Window m_window;

            //! Handle to GPU used for graphics processing
            SDL::GpuDevice m_gpu;

            //! Fill draw mode
            SDL::GraphicsPipeline m_fillpipeline;

            //! Wireframe draw mode.
            SDL::GraphicsPipeline m_wirepipeline;

            //! Text Rendering Pipeline.
            SDL::GraphicsPipeline m_textpipeline;

            //! Vertex Buffer.
            SDL::GpuBuffer m_vertexbuffer;

            //! Index Buffer.
            SDL::GpuBuffer m_indexbuffer;

            //! Transfer Buffer.
            SDL::GpuTransferBuffer m_transferbuffer;

            //! Sampler
            SDL::GpuSampler m_sampler;

            //! Geometry data
            GeometryData m_geometry_data;

            //! TTF Context.
            SDL::TTF::Context m_ttf;

            //! Font
            SDL::TTF::Font m_font;

            //! Text Engine
            SDL::TTF::TextEngine m_textengine;

            //! Text
            SDL::TTF::Text m_text;

            //! Uniforms
            UniformData m_uniform;
    };
}