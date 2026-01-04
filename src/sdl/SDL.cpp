#include "SDL.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <stdexcept>
#include <sstream>
#include <utility>

#include "core/Engine.hpp"
#include "core/Logger.hpp"
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


Uint64 SDL::Context::GetTicks() { // NOLINT depends on SDL_Init
    return SDL_GetTicks();
}

void SDL::Context::ShowCursor() { // NOLINT

    if (!SDL_ShowCursor()) {
        throw Error("SDL_ShowCursor() failed!");
    }
}

void SDL::Context::HideCursor() { // NOLINT
    if (!SDL_HideCursor()) {
        throw Error("SDL_HideCursor() failed!");
    }
}

bool SDL::Context::CursorVisible() { // NOLINT
    return SDL_CursorVisible();
}

std::string SDL::Context::GetPrefPath(const std::string& org, const std::string& app) { // NOLINT(readability-convert-member-functions-to-static)
    std::string path;

    char* p_prefPath = SDL_GetPrefPath(org.c_str(), app.c_str());
    if (p_prefPath == nullptr) {
        throw SDL::Error("SDL_GetPrefPath() failed!");
    }

    path = p_prefPath;
    SDL_free(p_prefPath);
    return path;
}

//----------------------------------------------------------------------------------------------------------------------
// SDL Surface

SDL::Image::Image(const std::string& filename)
    : m_p_surface(IMG_Load(filename.c_str())){
    if (m_p_surface == nullptr) {
        throw SDL::Error("IMG_Load() failed!");
    }
}

SDL::Image::Image(Image&& other) noexcept
    : m_p_surface(other.m_p_surface) {
    other.m_p_surface = nullptr;
}

SDL::Image& SDL::Image::operator=(Image&& other) noexcept {
    std::swap(m_p_surface, other.m_p_surface);
    return *this;
}

SDL::Image::~Image() {
    if (m_p_surface != nullptr) {
        SDL_DestroySurface(m_p_surface);
    }
}

void SDL::Image::SavePNG(const std::string& filename) {

    if (m_p_surface != nullptr) {
        if(!IMG_SavePNG(m_p_surface, filename.c_str())) {
            throw SDL::Error("IMG_SavePNG() failed!");
        }
    }
}

void SDL::Image::SaveJPG(const std::string& filename, int quality) {

    if (m_p_surface != nullptr) {
        if (!IMG_SaveJPG(m_p_surface, filename.c_str(), quality)) {
            throw SDL::Error("IMG_SaveJPG() failed!");
        }
    }
}

SDL_SurfaceFlags SDL::Image::GetFlags() const {
    return m_p_surface->flags;
}

SDL_PixelFormat SDL::Image::GetFormat() const {
    return m_p_surface->format;
}

uint32_t SDL::Image::GetWidth() const {
    return m_p_surface->w;
}

uint32_t SDL::Image::GetHeight() const {
    return m_p_surface->h;
}

uint32_t SDL::Image::GetPitch() const {
    return m_p_surface->pitch;
}

uint32_t SDL::Image::GetNumChannels() const {
    return m_p_surface->pitch / m_p_surface->w;
}

void* SDL::Image::GetPixels() const {
    return m_p_surface->pixels;
}

//----------------------------------------------------------------------------------------------------------------------
// SDL Window

SDL::Window::Window()
    : m_p_window(nullptr) {
}

SDL::Window::Window(const char* title, int width, int height, SDL_WindowFlags flags)
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

const SDL_Window* SDL::Window::Get() const {
    return m_p_window;
}

void SDL::Window::SetPosition(int width, int height) {

    if (!SDL_SetWindowPosition(m_p_window, width, height)) {

        throw Error("SDL_SetWindowPosition() failed!");
    }
}

void SDL::Window::SetFullscreen(bool fullscreen) {

    if (!SDL_SetWindowFullscreen(m_p_window, fullscreen)) {
        throw Error("SDL_SetWindowFullscreen() failed!");
    }
}

bool SDL::Window::GetFullscreen() const {
    SDL_WindowFlags flags = SDL_GetWindowFlags(m_p_window);

    return ((flags & SDL_WINDOW_FULLSCREEN) > 0U);
}

const SDL_DisplayMode* SDL::Window::GetFullscreenMode() {

    // can be null if in borderless desktop mode.
    return SDL_GetWindowFullscreenMode(m_p_window);
}

void SDL::Window::SetFullscreenMode(const SDL_DisplayMode& mode) {

    if(!SDL_SetWindowFullscreenMode(m_p_window, &mode)) {
        throw Error("SDL_SetWindowFullscreenMode() failed!");
    }
}

glm::ivec2 SDL::Window::GetWindowSize() const {

    glm::ivec2 windowSize(0.0F);
    if(!SDL_GetWindowSize(m_p_window, &windowSize.x, &windowSize.y)) {
        throw SDL::Error("Failed to retrieve SDL window size.");
    }
    return windowSize;
}

void SDL::Window::SetWindowSize(glm::ivec2 size) {

    if(!SDL_SetWindowSize(m_p_window, size.x, size.y)) {
        throw Error("SDL_SetWindowSize() failed!");
    }
}

void SDL::Window::StartTextInput() {
    if (!SDL_StartTextInput(m_p_window)) {
        throw Error("SDL_StartTextInput() failed!");
    }
}

void SDL::Window::StopTextInput() {
    if (!SDL_StopTextInput(m_p_window)) {
        throw Error("SDL_StopTextInput() failed!");
    }
}

bool SDL::Window::IsTextInputActive() const {
    return SDL_TextInputActive(m_p_window);
}

//----------------------------------------------------------------------------------------------------------------------
// SDL GPU

SDL::GpuDevice::GpuDevice(SDL_GPUShaderFormat format, bool debug, const char* driver)
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

SDL_GPUTextureFormat SDL::GpuDevice::GetSwapchainTextureFormat(Window& window) {
    return SDL_GetGPUSwapchainTextureFormat(m_p_gpu, window.Get());
}

SDL_GPUCommandBuffer* SDL::GpuDevice::AcquireGPUCommandBuffer() {
    SDL_GPUCommandBuffer* p_buffer = SDL_AcquireGPUCommandBuffer(m_p_gpu);
    if (p_buffer == nullptr) {
        throw SDL::Error("SDL_AcquireGPUCommandBuffer() failed!");
    }
    return p_buffer;
}

void SDL::GpuDevice::SubmitGPUCommandBuffer(SDL_GPUCommandBuffer* p_buffer) { // NOLINT
    if(!SDL_SubmitGPUCommandBuffer(p_buffer)) {
        throw SDL::Error("SDL_SubmitGPUCommandBuffer() failed!");
    }
}

SDL::GpuFence SDL::GpuDevice::SubmitGPUCommandBufferAndAcquireFence(SDL_GPUCommandBuffer* p_buffer) {

    SDL_GPUFence* p_fence = SDL_SubmitGPUCommandBufferAndAcquireFence(p_buffer);
    if (p_fence == nullptr) {
        throw SDL::Error("SDL_SubmitGPUCommandBufferAndAcquireFence() failed!");
    }
    return SDL::GpuFence(p_fence, m_p_gpu);
}

SDL_GPUDevice* SDL::GpuDevice::Get() {
    return m_p_gpu;
}

//----------------------------------------------------------------------------------------------------------------------
// SDL GPU Fence

SDL::GpuFence::GpuFence(SDL_GPUFence* p_fence, SDL_GPUDevice* p_device)
    : m_p_fence(p_fence)
    , m_p_device(p_device) {
}

SDL::GpuFence::GpuFence(GpuFence&& other) noexcept
    : m_p_fence(other.m_p_fence)
    , m_p_device(other.m_p_device) {
    other.m_p_fence = nullptr;
    other.m_p_device = nullptr;
}

SDL::GpuFence& SDL::GpuFence::operator=(GpuFence&& other) noexcept {
    std::swap(m_p_fence, other.m_p_fence);
    std::swap(m_p_device, other.m_p_device);
    return *this;
}

SDL::GpuFence::~GpuFence() {
    if (m_p_fence != nullptr) {
        Release();
    }
}

bool SDL::GpuFence::IsValid() {
    return (m_p_fence != nullptr && m_p_device != nullptr);
}

void SDL::GpuFence::Release() {

    SDL_ReleaseGPUFence(m_p_device, m_p_fence);
    m_p_device = nullptr;
    m_p_fence = nullptr;
}

void SDL::GpuFence::WaitFor() {

    if (!SDL_WaitForGPUFences(m_p_device, true, &m_p_fence, 1U)) {
        throw SDL::Error("SDL_WaitForGPUFences() failed!");
    }
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
        bytecode.GetNumSamplers(),
        0,
        0,
        bytecode.GetNumUniformBuffers(),
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


//----------------------------------------------------------------------------------------------------------------------
// SDL GPU Buffer

SDL::GpuBuffer::GpuBuffer()
    : m_p_device(nullptr)
    , m_p_buffer(nullptr) {
}

SDL::GpuBuffer::GpuBuffer(GpuDevice& gpu, const SDL_GPUBufferCreateInfo& createinfo)
    : m_p_buffer(SDL_CreateGPUBuffer(gpu.Get(), &createinfo))
    , m_p_device(&gpu) {

    if (m_p_buffer == nullptr) {
        throw SDL::Error("SDL_CreateGPUBuffer() failed.");
    }
}

SDL::GpuBuffer::GpuBuffer(GpuBuffer&& other) noexcept
    : m_p_buffer(other.m_p_buffer)
    , m_p_device(other.m_p_device) {
    other.m_p_buffer = nullptr;
    other.m_p_device = nullptr;
}

SDL::GpuBuffer& SDL::GpuBuffer::operator=(GpuBuffer&& other) noexcept {
    SDL_GPUBuffer* p_buffer = other.m_p_buffer;
    GpuDevice* p_device = other.m_p_device;

    other.m_p_device = m_p_device;
    other.m_p_buffer = m_p_buffer;

    m_p_buffer = p_buffer;
    m_p_device = p_device;

    return *this;
}

SDL::GpuBuffer::~GpuBuffer() {

    if (m_p_buffer != nullptr) {
        SDL_ReleaseGPUBuffer(m_p_device->Get(), m_p_buffer);
    }
}

SDL_GPUBuffer* SDL::GpuBuffer::Get() {

    return m_p_buffer;
}

void SDL::GpuBuffer::SetBufferName(const std::string& name) {

    SDL_SetGPUBufferName(m_p_device->Get(), m_p_buffer, name.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
// SDL GPU Transfer Buffer

SDL::GpuTransferBuffer::GpuTransferBuffer()
    : m_p_buffer(nullptr)
    , m_p_device(nullptr) {
}

SDL::GpuTransferBuffer::GpuTransferBuffer(GpuDevice& gpu, const SDL_GPUTransferBufferCreateInfo& createinfo)
    : m_p_buffer(SDL_CreateGPUTransferBuffer(gpu.Get(), &createinfo))
    , m_p_device(&gpu) {
    if (m_p_buffer == nullptr) {
        throw Error("SDL_CreateGPUTransferBuffer() failed.");
    }
}

SDL::GpuTransferBuffer::GpuTransferBuffer(GpuTransferBuffer&& other) noexcept
    : m_p_buffer(other.m_p_buffer)
    , m_p_device(other.m_p_device) {
    other.m_p_buffer = nullptr;
    other.m_p_device = nullptr;
}

SDL::GpuTransferBuffer& SDL::GpuTransferBuffer::operator=(SDL::GpuTransferBuffer&& other) noexcept {

    SDL_GPUTransferBuffer* p_buffer = other.m_p_buffer;
    GpuDevice* p_device = other.m_p_device;

    other.m_p_device = m_p_device;
    other.m_p_buffer = m_p_buffer;

    m_p_buffer = p_buffer;
    m_p_device = p_device;

    return *this;
}

SDL::GpuTransferBuffer::~GpuTransferBuffer() {

    if (m_p_buffer != nullptr) {
        SDL_ReleaseGPUTransferBuffer(m_p_device->Get(), m_p_buffer);
    }
}

SDL_GPUTransferBuffer* SDL::GpuTransferBuffer::Get() {
    return m_p_buffer;
}

void* SDL::GpuTransferBuffer::Map(bool cycle) {
    void* p_mapped = SDL_MapGPUTransferBuffer(m_p_device->Get(), m_p_buffer, cycle);
    if (p_mapped == nullptr) {
        throw SDL::Error("SDL_MapGPUTransferBuffer() failed!");
    }
    return p_mapped;
}

void SDL::GpuTransferBuffer::Unmap() {
    SDL_UnmapGPUTransferBuffer(m_p_device->Get(), m_p_buffer);
}

//----------------------------------------------------------------------------------------------------------------------
// SDL GPU Texture

SDL::GpuTexture::GpuTexture()
    : m_p_texture(nullptr)
    , m_p_device(nullptr) {
}

SDL::GpuTexture::GpuTexture(GpuDevice& device, const SDL_GPUTextureCreateInfo& createinfo)
    : m_p_texture(SDL_CreateGPUTexture(device.Get(), &createinfo))
    , m_p_device(&device) {

    if (m_p_texture == nullptr) {
        throw Error("SDL_CreateGPUTexture() failed.");
    }
}

SDL::GpuTexture::GpuTexture(SDL_GPUTexture* p_texture)
    : m_p_texture(p_texture)
    , m_p_device(nullptr) {
}

SDL::GpuTexture::GpuTexture(GpuTexture&& other) noexcept
    : m_p_texture(other.m_p_texture)
    , m_p_device(other.m_p_device) {

    other.m_p_device = nullptr;
    other.m_p_texture = nullptr;
}

SDL::GpuTexture& SDL::GpuTexture::operator=(GpuTexture&& other) noexcept {
    SDL_GPUTexture* p_texture = other.m_p_texture;
    GpuDevice* p_device = other.m_p_device;
    other.m_p_texture = m_p_texture;
    other.m_p_device = m_p_device;
    m_p_texture = p_texture;
    m_p_device = p_device;

    return *this;
}

SDL::GpuTexture::~GpuTexture() {

    if ((m_p_texture != nullptr) && (m_p_device != nullptr)) {
        SDL_ReleaseGPUTexture(m_p_device->Get(), m_p_texture);
    }
}


SDL_GPUTexture* SDL::GpuTexture::Get() {
    return m_p_texture;
}

void SDL::GpuTexture::SetName(const std::string& name) {
    if ((m_p_texture != nullptr) && (m_p_device != nullptr)) {
        SDL_SetGPUTextureName(m_p_device->Get(), m_p_texture, name.c_str());
    }
}

//----------------------------------------------------------------------------------------------------------------------
// SDL GPU Sampler

SDL::GpuSampler::GpuSampler()
    : m_p_sampler(nullptr)
    , m_p_device(nullptr) {
}

SDL::GpuSampler::GpuSampler(GpuDevice& device, const SDL_GPUSamplerCreateInfo& createinfo)
    : m_p_sampler(SDL_CreateGPUSampler(device.Get(), &createinfo))
    , m_p_device(&device) {

    if (m_p_sampler == nullptr) {
        throw Error("SDL_CreateGPUSampler() failed.");
    }
}

SDL::GpuSampler::GpuSampler(GpuSampler&& other) noexcept
    : m_p_sampler(other.m_p_sampler)
    , m_p_device(other.m_p_device) {

    other.m_p_device = nullptr;
    other.m_p_sampler = nullptr;
}

SDL::GpuSampler& SDL::GpuSampler::operator=(GpuSampler&& other) noexcept {
    SDL_GPUSampler* p_sampler = other.m_p_sampler;
    GpuDevice* p_device = other.m_p_device;
    other.m_p_sampler = m_p_sampler;
    other.m_p_device = m_p_device;
    m_p_sampler = p_sampler;
    m_p_device = p_device;

    return *this;
}

SDL::GpuSampler::~GpuSampler() {

    if (m_p_sampler != nullptr) {
        SDL_ReleaseGPUSampler(m_p_device->Get(), m_p_sampler);
    }
}

SDL_GPUSampler* SDL::GpuSampler::Get() {
    return m_p_sampler;
}
