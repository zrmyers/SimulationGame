#include "Mesh.hpp"
#include "components/Renderable.hpp"
#include "systems/RenderSystem.hpp"
#include <SDL3/SDL_gpu.h>
#include <cstdint>

namespace Graphics {

Mesh::Mesh(
    Systems::RenderSystem& renderer,
    std::shared_ptr<SDL::GpuBuffer> p_vertex_buffer,
    int32_t vertex_offset, size_t vertex_size,
    std::shared_ptr<SDL::GpuBuffer> p_index_buffer,
    SDL_GPUIndexElementSize index_size,
    uint32_t start_index,
    size_t num_indices)
    : m_p_render_system(&renderer)
    , m_vertex_buffer(std::move(p_vertex_buffer))
    , m_index_buffer(std::move(p_index_buffer))
    , m_vertex_offset(vertex_offset)
    , m_vertex_size(vertex_size)
    , m_start_index(start_index)
    , m_num_indices(num_indices)
    , m_index_size(index_size) {
    m_vertex_binding.buffer = m_vertex_buffer->Get();
    m_vertex_binding.offset = 0;
    m_index_binding.buffer = m_index_buffer->Get();
    m_index_binding.offset = 0;
}

int32_t Mesh::GetVertexOffset() const {
    return m_vertex_offset;
}

size_t Mesh::GetVertexSize() const {
    return m_vertex_size;
}

uint32_t Mesh::GetStartIndex() const {
    return m_start_index;
}

uint32_t Mesh::GetNumIndices() const {
    return m_num_indices;
}

SDL_GPUIndexElementSize Mesh::GetIndexSize() const {
    return m_index_size;
}

void Mesh::LoadData(void* p_vertex_data, size_t vertex_data_len, void* p_index_data, size_t index_data_len) {
    size_t indexSize = (m_index_size == SDL_GPU_INDEXELEMENTSIZE_16BIT)? sizeof(uint16_t) : sizeof(uint32_t);
    // setup transfer buffers
    std::vector<Components::TransferRequest> requestList;
    requestList.reserve(2);

    // vertex data
    {
        Components::TransferRequest request = {};
        request.cycle = false;
        request.type = Components::RequestType::UPLOAD_TO_BUFFER;
        request.data.buffer.buffer = m_vertex_buffer->Get();
        request.data.buffer.size = vertex_data_len;
        request.data.buffer.offset = 0;
        request.p_src = p_vertex_data;
        requestList.push_back(request);
    }

    {
        Components::TransferRequest request = {};
        request.cycle = false;
        request.type = Components::RequestType::UPLOAD_TO_BUFFER;
        request.data.buffer.buffer = m_index_buffer->Get();
        request.data.buffer.offset = 0;
        request.data.buffer.size = index_data_len;
        request.p_src = p_index_data;
        requestList.push_back(request);
    }

    m_p_render_system->UploadDataToBuffer(requestList);
}

const SDL_GPUBufferBinding& Mesh::GetVertexBufferBinding() const {
    return m_vertex_binding;
}

const SDL_GPUBufferBinding& Mesh::GetIndexBufferBinding() const {
    return m_index_binding;
}

}

