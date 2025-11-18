#include "Engine.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <cstddef>
#include <glslang/Public/ShaderLang.h>
#include <sstream>
#include <string>
#include "Logger.hpp"
#include "graphics/ShaderCross.hpp"
#include "sdl/SDL.hpp"

Core::EngineException::EngineException(const std::string& msg)
    : m_msg(msg) {
}

const char* Core::EngineException::what() const {
    return m_msg.c_str();
}

Core::Engine::Engine(const std::list<const char*>& args)
    : m_env(args)
    , m_sdl(SDL_INIT_VIDEO)
    , m_window(m_sdl, "Simulation Game", 1024, 768, SDL_WINDOW_VULKAN)
    , m_gpu(m_sdl, SDL_GPU_SHADERFORMAT_SPIRV, true, NULL) {

    m_window.SetPosition( SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    Core::Logger::Info("OK: Created device with driver: " + std::string(m_gpu.GetDriver()));

    m_gpu.ClaimWindow(m_window);

    std::string gamePath = m_env.Get("gamePath");

    // Load file from filesystem
    Graphics::ByteCode vertexShaderBytecode = m_shader_cross.CompileToSpirv(
        EShLangVertex, gamePath + "/content/shaders/simple.vert.glsl", "main", "/content/shaders", {});

    SDL::Shader vertexShader(vertexShaderBytecode, m_gpu);

    Graphics::ByteCode fragmentShaderBytecode = m_shader_cross.CompileToSpirv(
        EShLangFragment, gamePath + "/content/shaders/simple.frag.glsl", "main", "/content/shaders", {});

    SDL::Shader fragmentShader(fragmentShaderBytecode, m_gpu);

    std::vector<SDL_GPUColorTargetDescription> descriptions = {
        {SDL_GetGPUSwapchainTextureFormat(m_gpu.Get(), m_window.Get())}};

    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.target_info.num_color_targets = 1;
    pipelineCreateInfo.target_info.color_target_descriptions = descriptions.data();
    pipelineCreateInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineCreateInfo.vertex_shader = vertexShader.Get();
    pipelineCreateInfo.fragment_shader = fragmentShader.Get();
    pipelineCreateInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;

    m_fill = SDL::GraphicsPipeline(
        m_gpu,
        pipelineCreateInfo);

    pipelineCreateInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_LINE;
    m_wire = SDL::GraphicsPipeline(
        m_gpu,
        pipelineCreateInfo
    );
}


Core::Engine::~Engine() {

    m_gpu.ReleaseWindow(m_window);
}


void Core::Engine::Run() {

    bool shouldQuit = false;

    SDL_GPUViewport smallViewPort = {160, 120, 320, 240, 0.1F, 1.0F}; // NOLINT
    while (!shouldQuit) {

        SDL_Event event;
        if (SDL_PollEvent(&event)) {

            switch (event.type) {

                case SDL_EVENT_QUIT:
                    shouldQuit = true;
                    break;

                default:
                    break;
            }
        }

        SDL_GPUCommandBuffer* p_cmdbuf = SDL_AcquireGPUCommandBuffer(m_gpu.Get());
        if (p_cmdbuf == nullptr) {
            throw SDL::Error("SDL_AcquireGPUCommandBuffer() failed! ");
        }

        SDL_GPUTexture* p_swapchainTexture = nullptr;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(p_cmdbuf, m_window.Get(), &p_swapchainTexture, nullptr, nullptr)) {
            throw SDL::Error("SDL_WaitAndAcquireGPUSwapchainTexture() failed!");
        }

        if (p_swapchainTexture != nullptr) {

            SDL_GPUColorTargetInfo colorTargetInfo = {0};
            colorTargetInfo.texture = p_swapchainTexture;
            colorTargetInfo.clear_color = SDL_FColor{0.0, 0.0, 0.0, 1.0};
            colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
            colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

            SDL_GPURenderPass* p_renderPass = SDL_BeginGPURenderPass(p_cmdbuf, &colorTargetInfo, 1U, nullptr);
            SDL_BindGPUGraphicsPipeline(p_renderPass, m_fill.Get());

            SDL_DrawGPUPrimitives(p_renderPass, 3, 1, 0, 0);
            SDL_EndGPURenderPass(p_renderPass);
        }

        SDL_SubmitGPUCommandBuffer(p_cmdbuf);
    }
}

