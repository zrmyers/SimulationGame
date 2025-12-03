#pragma once

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <cstddef>
#include <exception>
#include <string>

#include <glm/vec2.hpp>

namespace Graphics {
    class ByteCode;
}

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

            //! Get the total amount of time in milliseconds that has elapsed since initialization.
            Uint64 GetTicks();

            //! Show the system cursor.
            void ShowCursor();

            //! Hide the system cursor.
            void HideCursor();

            //! Check if system cursor is currently visible.
            bool CursorVisible();
    };

    //! Wrapper around SDL surface object which holds image data.
    class Image {

        public:

            Image(const std::string& filename);
            Image(const Image&) = delete;
            Image(Image&& other) noexcept;
            Image& operator=(const Image&) = delete;
            Image& operator=(Image&& other) noexcept;
            ~Image();

            void SavePNG(const std::string& filename);

            //! See SDL_SaveJPG for docs. quality controls lossiness of jpg compression
            void SaveJPG(const std::string& filename, int quality);

            SDL_SurfaceFlags GetFlags() const;
            SDL_PixelFormat GetFormat() const;
            uint32_t GetWidth() const;
            uint32_t GetHeight() const;
            uint32_t GetPitch() const;
            uint32_t GetNumChannels() const;
            void* GetPixels() const;
            size_t GetSize() const;
        private:

            SDL_Surface* m_p_surface;
    };

    class Window {

        public:
            struct CreateInfo {
                const char* title;
                int width;
                int height;
                SDL_WindowFlags flags;
            };

            Window(const char* title, int width, int height, SDL_WindowFlags flags);
            Window(const Window& other) = delete;
            Window(Window&& other) noexcept;
            Window& operator=(const Window& other) = delete;
            Window& operator=(Window&& other) noexcept;
            ~Window();

            SDL_Window* Get();
            const SDL_Window* Get() const;

            //! Set the position of the window.
            void SetPosition(int width, int height);

            //! Get the size of the window.
            glm::ivec2 GetWindowSize() const;

        private:

            SDL_Window* m_p_window;
    };

    class GpuDevice {

        public:

            GpuDevice(SDL_GPUShaderFormat format, bool debug, const char* driver);
            GpuDevice(const GpuDevice& other) = delete;
            GpuDevice(GpuDevice&& other) noexcept;
            GpuDevice& operator=(const GpuDevice& other) = delete;
            GpuDevice& operator=(GpuDevice&& other) noexcept;
            ~GpuDevice();

            const char* GetDriver();

            //! Claim Window for the GPU.
            void ClaimWindow(Window& window);

            //! Release the window from the GPU.
            void ReleaseWindow(Window& window);

            SDL_GPUTextureFormat GetSwapchainTextureFormat(Window& window);

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
            //! @param[in] bytecode The bytecode with which to create the shader.
            //! @param[in] gpu    The parent gpu of the shader.
            Shader(const Graphics::ByteCode& bytecode, GpuDevice& gpu);
            Shader(const Shader& other) = delete;
            Shader(Shader&& other) noexcept;
            Shader& operator=(const Shader& other) = delete;
            Shader& operator=(Shader&& other) noexcept;
            ~Shader();

            SDL_GPUShader* Get();
        private:

            SDL_GPUShader* m_p_shader;
            GpuDevice* m_p_gpu;
    };

    class GraphicsPipeline {

        public:

            GraphicsPipeline();
            //! Initialize with existing pointer.
            //!
            //! @param[in] gpu    The parent gpu of the shader.
            GraphicsPipeline(GpuDevice& gpu, SDL_GPUGraphicsPipelineCreateInfo& createinfo);
            GraphicsPipeline(const GraphicsPipeline& other) = delete;
            GraphicsPipeline(GraphicsPipeline&& other) noexcept;
            GraphicsPipeline& operator=(const GraphicsPipeline& other) = delete;
            GraphicsPipeline& operator=(GraphicsPipeline&& other) noexcept;
            ~GraphicsPipeline();

            SDL_GPUGraphicsPipeline* Get();
        private:

            SDL_GPUGraphicsPipeline* m_p_pipeline;
            GpuDevice* m_p_gpu;
    };

    class GpuBuffer {

        public:
            GpuBuffer();
            GpuBuffer(GpuDevice& gpu, const SDL_GPUBufferCreateInfo& createinfo);
            GpuBuffer(const GpuBuffer& other) = delete;
            GpuBuffer(GpuBuffer&& other) noexcept;
            GpuBuffer& operator=(const GpuBuffer& other) = delete;
            GpuBuffer& operator=(GpuBuffer&& other) noexcept;
            ~GpuBuffer();

            SDL_GPUBuffer* Get();

            void SetBufferName(const std::string& name);

        private:
            SDL_GPUBuffer* m_p_buffer;
            GpuDevice* m_p_device;
    };

    class GpuTransferBuffer {

        public:
            GpuTransferBuffer();
            GpuTransferBuffer(GpuDevice& gpu, const SDL_GPUTransferBufferCreateInfo& createinfo);
            GpuTransferBuffer(const GpuTransferBuffer& other) = delete;
            GpuTransferBuffer(GpuTransferBuffer&& other) noexcept;
            GpuTransferBuffer& operator=(const GpuTransferBuffer& other) = delete;
            GpuTransferBuffer& operator=(GpuTransferBuffer&& other) noexcept;
            ~GpuTransferBuffer();

            SDL_GPUTransferBuffer* Get();
            //! Make gpu memory accessible to CPU.
            void* Map(bool cycle = false);
            void Unmap();

        private:
            SDL_GPUTransferBuffer* m_p_buffer;
            GpuDevice* m_p_device;
    };

    class GpuTexture {

        public:
            GpuTexture();

            GpuTexture(GpuDevice& device, const SDL_GPUTextureCreateInfo& createinfo); // owning constructor
            GpuTexture(SDL_GPUTexture* p_texture); // non-owning constructor
            GpuTexture(const GpuTexture& other) = delete;
            GpuTexture(GpuTexture&& other) noexcept;
            GpuTexture& operator=(const GpuTexture& other) = delete;
            GpuTexture& operator=(GpuTexture&& other) noexcept;
            ~GpuTexture();

            SDL_GPUTexture* Get();

            void SetName(const std::string& name);

        private:

            SDL_GPUTexture* m_p_texture;
            GpuDevice* m_p_device;
    };

    class GpuSampler {

        public:
            GpuSampler();
            GpuSampler(GpuDevice& device, const SDL_GPUSamplerCreateInfo& createinfo);
            GpuSampler(const GpuSampler& other) = delete;
            GpuSampler(GpuSampler&& other) noexcept;
            GpuSampler& operator=(const GpuSampler& other) = delete;
            GpuSampler& operator=(GpuSampler&& other) noexcept;
            ~GpuSampler();

            SDL_GPUSampler* Get();
        private:

            SDL_GPUSampler* m_p_sampler;
            GpuDevice* m_p_device;
    };

}