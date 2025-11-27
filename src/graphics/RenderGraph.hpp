#pragma once

#include "sdl/SDL.hpp"
#include "systems/RenderSystem.hpp"
#include <SDL3/SDL_gpu.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "UploadData.hpp"

namespace Graphics {

    enum class RGResourceType : uint8_t {
        BUFFER,
        TEXTURE
    };

    class IRGResource {

        public:

            // tranisent resources are rebuilt every frame. non-transient are reused.
            IRGResource(RGResourceType type) : m_type(type) {};
            IRGResource(const IRGResource&) = default;
            IRGResource(IRGResource&&) = default;
            IRGResource& operator=(const IRGResource&) = default;
            IRGResource& operator=(IRGResource&&) noexcept = default;
            virtual ~IRGResource() = 0;

            RGResourceType GetType() const {return m_type;};

            virtual void Allocate(Systems::RenderSystem& renderSys) = 0;
            virtual void Bind(SDL_GPURenderPass& renderpass) = 0;
        private:

            RGResourceType m_type;
    };

    // Buffer resource
    class RGBuffer : public IRGResource {

        public:

            //! Information required for requesting an allocation.
            RGBuffer(SDL::GpuBuffer&& external_buffer);
            RGBuffer(SDL_GPUBufferUsageFlags usageFlags, uint32_t num_elements, size_t element_size);
            RGBuffer(const RGBuffer&) = delete;
            RGBuffer(RGBuffer&&) = default;
            RGBuffer& operator=(const RGBuffer&) = delete;
            RGBuffer& operator=(RGBuffer&&) = default;
            ~RGBuffer() override;

            //! Return the usage flags of the buffer.
            SDL_GPUBufferUsageFlags GetUsageFlags();

            //! Return the max element count of the buffer.
            uint32_t GetMaxElementCount();

            //! Return the size, in bytes, of each element in the buffer.
            uint32_t GetElementSize();

            //! Return the offset, in bytes, of data in the buffer.
            uint32_t GetBufferOffset();

            //! Get a handle to the underlying resource.
            SDL_GPUBuffer* GetBufferHandle();

            //! Used to grab buffer allocation from pool
            void Allocate(Systems::RenderSystem& renderSys) override;

        private:

            //! Usage flags for the buffer.
            SDL_GPUBufferUsageFlags m_usage;

            //! Maximum number of elements in the buffer.
            uint32_t m_num_elements;

            //! Size of each element in the buffer.
            size_t m_element_size;

            //! The GPU buffer handle.
            SDL::GpuBuffer m_p_buffer;

            //! Offset into buffer.
            uint32_t m_buffer_offset;
    };

    //! A sampled-texture object.
    class RGTexture : public IRGResource {

        public:

            //! Information required for requesting an allocation.
            RGTexture(SDL_GPUSamplerCreateInfo& sampler_create_info, SDL_GPUTextureCreateInfo& texture_create_info);
            RGTexture(SDL_GPUSamplerCreateInfo& sampler_create_info, SDL::GpuTexture& external_texture);
            RGTexture(SDL::GpuSampler&& external_sampler, SDL_GPUTextureCreateInfo& texture_create_info);
            RGTexture(SDL::GpuSampler&& external_sampler, SDL::GpuTexture&& external_texture);
            RGTexture(const RGTexture&) = delete;
            RGTexture(RGTexture&&) = default;
            RGTexture& operator=(const RGTexture&) = delete;
            RGTexture& operator=(RGTexture&&) = default;
            ~RGTexture() override;

            void Allocate(Systems::RenderSystem& renderSys) override;

        private:

            //! Used for allocating sample or texture.
            SDL_GPUSamplerCreateInfo m_sampler_info;
            SDL_GPUTextureCreateInfo m_texture_info;

            //! Pointer to the sampler handle.
            SDL::GpuSampler p_sampler;

            //! Pointer to the texture handle.
            SDL::GpuTexture p_texture;
    };

    //!
    //! RGBuilder is responsible for compiling various render passes into sorted command lists, and executing.
    class RGBuilder {

        public:

            //! Instantiate the builder.
            RGBuilder();

            //! Create a vertex/index buffer. This creates a future allocation of vertex/index data on the GPU.
            //!
            //! The vertex/index data is allocated from a pool of vertex buffers during the next pipeline pass. A weak ptr to
            //! to the buffer is returned. If the buffer should be persisted, the weak pointer can be transformed to
            //! shared pointer to reuse the buffer for the next frame.
            //!
            //! @param[in] num_elements The number of elements in the buffer.
            //! @param[in] element_size The size of each element in the buffer.
            //!
            //! Vertex buffer may be target of buffer upload.
            std::weak_ptr<RGBuffer> CreateVertexBuffer(SDL_GPUBufferUsageFlags usage, uint32_t num_elements, size_t element_size);

            //! Create a texture-sampler. This creates a future allocation for sampler/texture on the GPU.
            //!
            //! A sampler and texture pool is used to allocate the texture-sampler object.
            //!
            //! @param[in] samplerInfo Information for creating the sampler.
            //! @param[in] textureInfo Information for creating the texture.
            std::weak_ptr<RGTexture> CreateTexture(SDL_GPUSamplerCreateInfo& sampler_info, SDL_GPUTextureCreateInfo& texture_info);
            std::weak_ptr<RGTexture> CreateTexture(SDL_GPUSamplerCreateInfo& sampler_info, SDL::GpuTexture&& texture);

            //! Register an externally managed buffer.
            void RegisterExternalBuffer(std::shared_ptr<RGBuffer> p_buffer);
            void RegisterExternalTexture(std::shared_ptr<RGTexture> p_sampler);

            //! Queue future upload of data to buffer.
            void QueueBufferUpload(RGBuffer& buffer, const IRGUploadData& uploadData); // User keeps data in scope before execution
            void QueueBufferUpload(RGBuffer& buffer, IRGUploadData&& uploadData); // User gives rendergraph ownership of data.

            //! Queue future upload of data to texture.
            void QueueTextureUpload(RGBuffer& buffer, const IRGUploadData& uploadData); // User keeps data in scope before execution
            void QueueTextureUpload(RGBuffer& buffer, IRGUploadData&& uploadData); // User gives rendergraph ownership of data.

        private:

    };
}