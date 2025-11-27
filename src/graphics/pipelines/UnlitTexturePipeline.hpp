#pragma once

#include "graphics/ShaderCross.hpp"
#include "sdl/SDL.hpp"
#include "systems/RenderSystem.hpp"
#include "PipelineCache.hpp"

namespace Graphics {

    struct UnlitTexturedVertex {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texcoord;
    };

    // Main pipeline for rendering without lighting.
    //
    // Primarily used by UI Render Layer for sprites and text. An SDF variant is provided
    class UnlitTexturePipeline : public IPipeline {

        public:

            UnlitTexturePipeline()
                : m_vertex_name("guiText.vert.glsl")
                , m_frag_name("guiText.frag.glsl") {
            };

            UnlitTexturePipeline(std::string vertex_name, std::string frag_name)
                : m_vertex_name(std::move(vertex_name))
                , m_frag_name(std::move(frag_name)) {
            }

            void Build(Systems::RenderSystem& rendersys, const std::string& shaderpath) override {

                Graphics::ShaderCross compiler;

                // Load file from filesystem
                Graphics::ByteCode vertexShaderBytecode = compiler.CompileToSpirv(
                    EShLangVertex, shaderpath + "/" + m_vertex_name, "main", shaderpath, {});

                SDL::Shader vertexShader = rendersys.CreateShader(vertexShaderBytecode);

                Graphics::ByteCode fragmentShaderBytecode = compiler.CompileToSpirv(
                    EShLangFragment, shaderpath + "/" + m_frag_name, "main", shaderpath, {});

                SDL::Shader fragmentShader = rendersys.CreateShader(fragmentShaderBytecode);

                SDL_GPUGraphicsPipelineCreateInfo pipelinecreateinfo = {};

                //------------------------------------------------------------------------------------------------------------------
                // Target Info Description

                SDL_GPUGraphicsPipelineTargetInfo& targetInfo = pipelinecreateinfo.target_info;

                SDL_GPUColorTargetDescription colorTargetDescription;
                colorTargetDescription.format = rendersys.GetSwapchainTextureFormat();
                colorTargetDescription.blend_state.enable_blend = true,
                colorTargetDescription.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                colorTargetDescription.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD,
                colorTargetDescription.blend_state.color_write_mask = 0xF, // NOLINT
                colorTargetDescription.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                colorTargetDescription.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_DST_ALPHA,
                colorTargetDescription.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                colorTargetDescription.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

                targetInfo.num_color_targets = 1;
                targetInfo.color_target_descriptions = & colorTargetDescription;
                targetInfo.has_depth_stencil_target = false;
                targetInfo.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_INVALID;

                //------------------------------------------------------------------------------------------------------------------
                // Vertex Input State
                SDL_GPUVertexInputState& vertexInputState = pipelinecreateinfo.vertex_input_state;
                // all attributes are interleaved in same structure
                SDL_GPUVertexBufferDescription vertexBufferDescription = {};
                vertexBufferDescription.slot = 0;
                vertexBufferDescription.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
                vertexBufferDescription.instance_step_rate = 0;
                vertexBufferDescription.pitch = sizeof(UnlitTexturedVertex);

                vertexInputState.vertex_buffer_descriptions = &vertexBufferDescription;
                vertexInputState.num_vertex_buffers = 1;

                // TODO should be able to reflect this from the vertex shader bytecode.
                std::array<SDL_GPUVertexAttribute, 3> vertexAttributes = {};
                vertexAttributes[0].buffer_slot = 0;
                vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
                vertexAttributes[0].location = 0;
                vertexAttributes[0].offset = offsetof(UnlitTexturedVertex, position);
                vertexAttributes[1].buffer_slot = 0;
                vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
                vertexAttributes[1].location = 1;
                vertexAttributes[1].offset = offsetof(UnlitTexturedVertex, color);
                vertexAttributes[2].buffer_slot = 0;
                vertexAttributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
                vertexAttributes[2].location = 2;
                vertexAttributes[2].offset = offsetof(UnlitTexturedVertex, texcoord);

                vertexInputState.vertex_attributes = vertexAttributes.data();
                vertexInputState.num_vertex_attributes = vertexAttributes.size();

                pipelinecreateinfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
                pipelinecreateinfo.vertex_shader = vertexShader.Get();
                pipelinecreateinfo.fragment_shader = fragmentShader.Get();

                // Create non-SDF shader
                Get() = rendersys.CreatePipeline(pipelinecreateinfo);
            }

        private:

            std::string m_vertex_name;
            std::string m_frag_name;
    };

    // Unlit texture pipeline, with fragment shader that performs signed distance field (SDF) calculations.
    //
    // This pipeline is used to render SDF fonts, which are better at maintaining clarity through various scaling and view
    // angles.
    class UnlitTextureSDFPipeline : public UnlitTexturePipeline {

        public:

            UnlitTextureSDFPipeline()
                : UnlitTexturePipeline("guiText.vert.glsl", "guiText-sdf.frag.glsl"){};
    };
}