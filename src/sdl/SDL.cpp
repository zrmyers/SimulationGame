#include "SDL.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <stdexcept>
#include <sstream>

#include "graphics/ShaderCross.hpp"

//----------------------------------------------------------------------------------------------------------------------
// SDL Error

SDL::Error::Error(const std::string& msg) {

    std::stringstream msgStream;

    msgStream << msg << ": " << SDL_GetError();

    m_msg = msgStream.str();

    SDL_ClearError();
}

const char * SDL::Error::what() const noexcept {
    return m_msg.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
// SDL Context

SDL::Context::Context(SDL_InitFlags flags) {
    if(!SDL_Init(flags)) {
        throw Error("SDL_Init() failed: ");
    }
}

SDL::Context::~Context() {
    SDL_Quit();
}

//----------------------------------------------------------------------------------------------------------------------
// SDL Window

SDL::Window::Window(Context& context, const char* title, int width, int height, SDL_WindowFlags flags)
    : m_p_window(
        SDL_CreateWindow(
            title,
             width,
             height,
             flags)) {

    if (m_p_window == nullptr) {
        throw Error("SDL_CreateWindow() returned nullptr ");
    }
}


SDL::Window::Window(Window&& other) noexcept
    : m_p_window(other.m_p_window) {
    other.m_p_window = nullptr;
}

SDL::Window& SDL::Window::operator=(Window&& other) noexcept {

    //! swap
    SDL_Window* p_window = m_p_window;
    m_p_window = other.m_p_window;
    other.m_p_window = p_window;

    return *this;
}

SDL::Window::~Window() {
    if (m_p_window != nullptr) {
        SDL_DestroyWindow(m_p_window);
    }
}

SDL_Window* SDL::Window::Get() {
    return m_p_window;
}

void SDL::Window::SetPosition(int width, int height) {

    if (!SDL_SetWindowPosition(m_p_window, width, height)) {

        throw Error("SDL_SetWindowPosition() failed!");
    }
}

//----------------------------------------------------------------------------------------------------------------------
// SDL GPU

SDL::GpuDevice::GpuDevice(Context& context, SDL_GPUShaderFormat format, bool debug, const char* driver)
    : m_p_gpu(SDL_CreateGPUDevice(format, debug, driver)) {
    if (m_p_gpu == nullptr) {
        throw Error("SDL_CreateGPUDevice() failed!");
    }
}

SDL::GpuDevice::GpuDevice(GpuDevice&& other) noexcept
    : m_p_gpu(other.m_p_gpu) {
    other.m_p_gpu = nullptr;
}

SDL::GpuDevice& SDL::GpuDevice::operator=(GpuDevice&& other) noexcept {
    SDL_GPUDevice* p_device = m_p_gpu;
    m_p_gpu = other.m_p_gpu;
    other.m_p_gpu = p_device;

    return *this;
}

SDL::GpuDevice::~GpuDevice() {
    SDL_DestroyGPUDevice(m_p_gpu);
}

const char* SDL::GpuDevice::GetDriver() {
    const char* p_name = SDL_GetGPUDeviceDriver(m_p_gpu);
    if (p_name == nullptr) {
        throw Error("SDL_GetGPUDeviceDriver() failed!");
    }
    return p_name;
}

void SDL::GpuDevice::ClaimWindow(Window& window) {

    if (!SDL_ClaimWindowForGPUDevice(m_p_gpu, window.Get())) {
        throw Error("SDL_ClaimWindowForGPUDevice() failed!");
    }
}


void SDL::GpuDevice::ReleaseWindow(Window& window) {

    SDL_ReleaseWindowFromGPUDevice(m_p_gpu, window.Get());
}


SDL_GPUDevice* SDL::GpuDevice::Get() {
    return m_p_gpu;
}


//----------------------------------------------------------------------------------------------------------------------
// SDL Shader

SDL::Shader::Shader()
    : m_p_shader(nullptr)
    , m_p_gpu(nullptr) {
}

SDL::Shader::Shader(const Graphics::ByteCode& bytecode, GpuDevice& gpu)
    : m_p_shader(nullptr)
    , m_p_gpu(&gpu) {

    SDL_GPUShaderCreateInfo shaderInfo = {
        bytecode.GetSize(),
        static_cast<const Uint8*>(bytecode.Get()),
        bytecode.GetEntrypoint(),
        bytecode.GetFormat(),
        bytecode.GetStage(),
        0,
        0,
        0,
        0,
    };

    m_p_shader = SDL_CreateGPUShader(m_p_gpu->Get(), &shaderInfo);
    if (m_p_shader == nullptr) {
        throw SDL::Error("SDL_CreateGPUShader() failed!");
    }
}

SDL::Shader::Shader(Shader&& other) noexcept
    : m_p_shader(other.m_p_shader)
    , m_p_gpu(other.m_p_gpu) {
    other.m_p_shader = nullptr;
    other.m_p_gpu = nullptr;
}

SDL::Shader& SDL::Shader::operator=(Shader&& other) noexcept {
    SDL_GPUShader* p_shader = m_p_shader;
    GpuDevice* p_gpu = m_p_gpu;

    m_p_gpu = other.m_p_gpu;
    m_p_shader = other.m_p_shader;

    other.m_p_gpu = p_gpu;
    other.m_p_shader = p_shader;

    return *this;
}

SDL::Shader::~Shader() {

    if (m_p_shader != nullptr) {

        SDL_ReleaseGPUShader(m_p_gpu->Get(), m_p_shader);
    }
}

SDL_GPUShader* SDL::Shader::Get() {
    return m_p_shader;
}

//----------------------------------------------------------------------------------------------------------------------
// SDL Graphics Pipeline

SDL::GraphicsPipeline::GraphicsPipeline()
    : m_p_gpu(nullptr)
    , m_p_pipeline(nullptr) {
}

SDL::GraphicsPipeline::GraphicsPipeline(GpuDevice& gpu, SDL_GPUGraphicsPipelineCreateInfo& createinfo)
    : m_p_pipeline(SDL_CreateGPUGraphicsPipeline(gpu.Get(), &createinfo))
    , m_p_gpu(&gpu) {

    if (m_p_pipeline == nullptr) {
        throw SDL::Error("SDL_CreateGPUGraphicsPipeline() failed!");
    }
}

SDL::GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& other) noexcept
    : m_p_pipeline(other.m_p_pipeline)
    , m_p_gpu(other.m_p_gpu) {
    other.m_p_pipeline = nullptr;
    other.m_p_gpu = nullptr;
}

SDL::GraphicsPipeline& SDL::GraphicsPipeline::operator=(GraphicsPipeline&& other) noexcept {

    SDL_GPUGraphicsPipeline* p_pipeline = m_p_pipeline;
    GpuDevice* p_device = m_p_gpu;

    m_p_pipeline = other.m_p_pipeline;
    m_p_gpu = other.m_p_gpu;
    other.m_p_pipeline = p_pipeline;
    other.m_p_gpu = p_device;

    return *this;
}

SDL::GraphicsPipeline::~GraphicsPipeline() {
    if (m_p_pipeline != nullptr) {
        SDL_ReleaseGPUGraphicsPipeline(m_p_gpu->Get(), m_p_pipeline);
    }
}

SDL_GPUGraphicsPipeline* SDL::GraphicsPipeline::Get() {
    return m_p_pipeline;
}