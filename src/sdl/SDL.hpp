#pragma once

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <string>
#include <unordered_map>

// Light-weight RAII wrapper for SDL library.
namespace SDL {

    // Forward declarations.
    class Error;
    class Context;
    class Window;
    class GpuDevice;
    class ShaderCross;
    class Shader;

    class Error : public std::exception {

        public:
            Error(const std::string& msg);

            const char * what() const noexcept override;

        private:

            std::string m_msg;
    };

    //! Wrapper around an SDL managed allocation.
    template<typename T>
    class Alloc {

        public:
            Alloc()
                : m_p_value(nullptr)
                , m_size(0) {
            }

            Alloc(void* p_value, size_t size)
                : m_p_value(p_value)
                , m_size(size) {
            }

            Alloc(const Alloc<T>& other) = delete;
            Alloc(Alloc&& other) = default;
            Alloc& operator=(const Alloc& other) = delete;
            Alloc& operator=(Alloc&& other) = default;

            ~Alloc() {
                if (m_p_value != nullptr) {
                    SDL_free(m_p_value);
                }
            }

            T* Get() {
                return reinterpret_cast<T*>(m_p_value); // NOLINT
            }

            const T* Get() const {
                return reinterpret_cast<const T*>(m_p_value); // NOLINT
            }

            size_t GetSize() const {
                return m_size;
            }

        private:

            void* m_p_value;
            size_t m_size;
    };

    class Context {

        public:
            Context(SDL_InitFlags flags);
            Context(const Context& other) = delete;
            Context(Context&& other) = default;
            Context& operator=(const Context& other) = delete;
            Context& operator=(Context&& other) = default;
            ~Context();

            // Filesystem related commands.
            Alloc<Uint8> LoadFile(const std::string& filename);
    };

    class Window {

        public:
            struct CreateInfo {
                const char* title;
                int width;
                int height;
                SDL_WindowFlags flags;
            };

            Window(Context& context, const char* title, int width, int height, SDL_WindowFlags flags);
            Window(const Window& other) = delete;
            Window(Window&& other) = default;
            Window& operator=(const Window& other) = delete;
            Window& operator=(Window&& other) = default;
            ~Window();

            SDL_Window* Get();

            //! Set the position of the window.
            void SetPosition(int width, int height);

        private:

            SDL_Window* m_p_window;
    };

    class GpuDevice {

        public:

            GpuDevice(Context& context, SDL_GPUShaderFormat format, bool debug, const char* driver);
            GpuDevice(const GpuDevice& other) = delete;
            GpuDevice(GpuDevice&& other) noexcept = default;
            GpuDevice& operator=(const GpuDevice& other) = delete;
            GpuDevice& operator=(GpuDevice&& other) = default;
            ~GpuDevice();

            const char* GetDriver();

            //! Claim Window for the GPU.
            void ClaimWindow(Window& window);

            //! Release the window from the GPU.
            void ReleaseWindow(Window& window);

            //! Get the SDL pointer.
            SDL_GPUDevice* Get();

        private:

            SDL_GPUDevice* m_p_gpu;
    };

    class Shader {

        public:

            Shader();
            //! Initialize with existing pointer.
            //!
            //! @param[in] p_shader The shader resource.
            //! @param[in] gpu    The parent gpu of the shader.
            Shader(SDL_GPUShader* p_shader, GpuDevice& gpu);
            Shader(const Shader& other) = delete;
            Shader(Shader&& other) = default;
            Shader& operator=(const Shader& other) = delete;
            Shader& operator=(Shader&& other) = default;
            ~Shader();

            SDL_GPUShader* Get();
        private:

            SDL_GPUShader* m_p_shader;
            GpuDevice* m_p_gpu;
    };
}