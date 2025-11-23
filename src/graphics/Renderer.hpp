#pragma once

#include "Renderable.hpp"
#include "core/AssetLoader.hpp"
#include "graphics/ShaderCross.hpp"
#include "sdl/SDL.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>
#include <cstdint>
#include <string>
#include <unordered_map>

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
        glm::mat4 projview{};
        glm::mat4 model{};
    };


    class Renderer {

        public:

            //! Initialize the renderer.
            Renderer(SDL::Context& context);
            Renderer(const Renderer& other) = delete;
            Renderer(Renderer&& other) = delete;
            Renderer& operator=(const Renderer& other) = delete;
            Renderer& operator=(Renderer&& other) = delete;
            ~Renderer();

            SDL::GpuDevice& GetGpuDevice();

            //! Get Swap chain texture format.
            SDL_GPUTextureFormat GetSwapchainTextureFormat();

            //! Create a shader.
            SDL::Shader CreateShader(Graphics::ByteCode& code);

            //! Create a pipeline for the GPU.
            SDL::GraphicsPipeline CreatePipeline(SDL_GPUGraphicsPipelineCreateInfo& pipeline_info);

            //! Create a buffer on the GPU.
            SDL::GpuBuffer CreateBuffer(SDL_GPUBufferUsageFlags usage, uint32_t size);

            //! Create a transfer buffer on the GPU.
            SDL::GpuTransferBuffer CreateTransferBuffer(SDL_GPUTransferBufferUsage usage, uint32_t size);

            //! Create a sampler on the GPU.
            SDL::GpuSampler CreateSampler(SDL_GPUSamplerCreateInfo& sampler_info);

            //! Main update loop.
            //!
            //! - perform transfers
            //! - draw
            void Update(std::vector<Renderable>& renderables, const Graphics::UniformData& uniform);

        private:

            //! Window used for rendering graphics to screen.
            SDL::Window m_window;

            //! Handle to GPU used for graphics processing
            SDL::GpuDevice m_gpu;
    };
}