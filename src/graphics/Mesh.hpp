#pragma once

#include "core/Engine.hpp"
#include "sdl/SDL.hpp"
#include <SDL3/SDL_gpu.h>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace Systems {
    class RenderSystem;
}

namespace Graphics {

    // Class which represents a generic mesh.
    class Mesh {

        public:
            Mesh(
                Systems::RenderSystem& renderer,
                std::shared_ptr<SDL::GpuBuffer> p_vertex_buffer,
                int32_t vertex_offset, size_t vertex_size,
                std::shared_ptr<SDL::GpuBuffer> p_index_buffer,
                SDL_GPUIndexElementSize index_size,
                uint32_t start_index,
                size_t num_indices);

            int32_t GetVertexOffset() const;
            size_t GetVertexSize() const;
            uint32_t GetStartIndex() const;
            uint32_t GetNumIndices() const;
            SDL_GPUIndexElementSize GetIndexSize() const;

            template<typename VertexType, typename IndexType>
            void LoadData(std::vector<VertexType>& vertices, std::vector<IndexType>& indices) {
                size_t expectedIndexSize = (m_index_size == SDL_GPU_INDEXELEMENTSIZE_16BIT)? sizeof(uint16_t) : sizeof(uint32_t);
                if (sizeof(IndexType) != expectedIndexSize) {
                    throw Core::EngineException("Failed to load vertex data. provided index type does not have expected size.");
                }
                if (m_vertex_size != sizeof(VertexType)) {
                    throw Core::EngineException("Failed to load vertex data. provided vertex type does not have expected size.");
                }

                LoadData(
                    static_cast<void*>(vertices.data()), vertices.size()*sizeof(VertexType),
                    static_cast<void*>(indices.data()), indices.size()*expectedIndexSize);
            }

            //! Load vertices/indices to GPU.
            void LoadData(void* p_vertex_data, size_t vertex_data_len,void* p_index_data, size_t index_data_len);

            //! Get Vertex Buffer Binding
            const SDL_GPUBufferBinding& GetVertexBufferBinding() const;

            //! Get Index Buffer Binding
            const SDL_GPUBufferBinding& GetIndexBufferBinding() const;

        private:

            Systems::RenderSystem* m_p_render_system;
            //! vertex and index buffer
            std::shared_ptr<SDL::GpuBuffer> m_vertex_buffer;
            std::shared_ptr<SDL::GpuBuffer> m_index_buffer;

            //! Vertex offset, starting offset of vertices to bind in vertex buffer.
            int32_t m_vertex_offset {0};

            // vertex size
            size_t m_vertex_size {0};

            //! Starting index of mesh in index buffer.
            uint32_t m_start_index {0U};

            //! Number of indices
            uint32_t m_num_indices {0U};

            //! Index size
            SDL_GPUIndexElementSize m_index_size {SDL_GPU_INDEXELEMENTSIZE_16BIT};

            SDL_GPUBufferBinding m_vertex_binding{};
            SDL_GPUBufferBinding m_index_binding{};
    };
}