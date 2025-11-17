#include "SDL.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <stdexcept>
#include <sstream>

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

SDL::Alloc<Uint8> SDL::Context::LoadFile(const std::string& filename) { // NOLINT
    size_t datasize = 0;
    void* p_data = SDL_LoadFile(filename.c_str(), &datasize);
    if (p_data == nullptr) {
        throw Error("SDL_LoadFile() failed!");
    }

    return SDL::Alloc<Uint8>(p_data, datasize);
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

SDL::Window::~Window() {

    SDL_DestroyWindow(m_p_window);
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
// SDL GPU

SDL::Shader::Shader()
    : m_p_shader(nullptr)
    , m_p_gpu(nullptr) {
}

SDL::Shader::Shader(SDL_GPUShader* p_shader, GpuDevice& gpu)
    : m_p_shader(p_shader)
    , m_p_gpu(&gpu) {

    if (m_p_gpu != nullptr) {
        throw std::runtime_error("Cannot initialize shader without gpu!");
    }

    if (m_p_shader != nullptr) {
        throw std::runtime_error("Cannot initialize shader without p shader!");
    }
}


SDL::Shader::~Shader() {

    if (m_p_shader != nullptr) {

        SDL_ReleaseGPUShader(m_p_gpu->Get(), m_p_shader);
    }
}