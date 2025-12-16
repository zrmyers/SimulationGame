#include "Texture2D.hpp"
#include "core/Engine.hpp"
#include "systems/RenderSystem.hpp"
#include <SDL3/SDL_gpu.h>
#include <cstdint>


Graphics::Texture2D::Texture2D(
    Core::Engine& engine,
    std::shared_ptr<SDL::GpuSampler> p_sampler,
    uint32_t width,
    uint32_t height,
    bool generate_mipmap)
    : m_p_render_system(&engine.GetEcsRegistry().GetSystem<Systems::RenderSystem>())
    , m_p_sampler {std::move(p_sampler)}
    , m_width(width)
    , m_height(height)
    , m_mipmaps(generate_mipmap) {

    SDL_GPUTextureCreateInfo createinfo = {};
    createinfo.type = SDL_GPU_TEXTURETYPE_2D;
    createinfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    createinfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    createinfo.width = width;
    createinfo.height = height;
    createinfo.layer_count_or_depth = 1;
    createinfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    createinfo.num_levels = 1;

    if (generate_mipmap) {

        createinfo.num_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1U;
    }

    m_texture = m_p_render_system->CreateTexture(createinfo);
}

void Graphics::Texture2D::LoadImageData(
    const SDL::Image& image, glm::ivec2 dst_offset) {

    // texture transfer
    Components::TransferRequest request = {};
    request.cycle = false;
    request.type = Components::RequestType::UPLOAD_TO_TEXTURE;
    SDL_GPUTextureRegion& region = request.data.texture;
    region.texture = m_texture.Get();
    region.w = image.GetWidth();
    region.h = image.GetHeight();
    region.d = 1;
    region.x = dst_offset.x;
    region.y = dst_offset.y;
    request.p_src = image.GetPixels();

    m_p_render_system->UploadDataToBuffer({request});

    if (m_mipmaps) {
        m_p_render_system->GenerateMipMaps(m_texture);
    }
}

void Graphics::Texture2D::LoadImageData(
    const SDL::Image& image, glm::uvec2 src_offset, glm::uvec2 src_extent, glm::ivec2 dst_offset) {

    if ((src_offset.x < 0)
        || (src_offset.y < 0)
        || (src_offset.x + src_extent.x > image.GetWidth())
        || (src_offset.y + src_extent.y > image.GetHeight())) {
        throw Core::EngineException("Texture2D::LoadImageData() src_offset and src_extent are not within bounds of image.");
    }

    if (image.GetNumChannels() != 4) {
        throw Core::EngineException("Texture2D::LoadImageData() num channels is not 4.");
    }

    // each pixel is 4 bytes
    uint32_t* pixelData = static_cast<uint32_t*>(image.GetPixels());
    std::vector<uint32_t> pixelRegion;

    uint32_t maxx = src_offset.x + src_extent.x;
    uint32_t maxy = src_offset.y + src_extent.y;

    for (uint32_t yIndex = src_offset.y; yIndex < maxy; yIndex++) {
        for (uint32_t xIndex = src_offset.x; xIndex < maxx; xIndex++) {
            uint32_t pixelIndex = xIndex + (yIndex * image.GetWidth());
            pixelRegion.push_back(pixelData[pixelIndex]);
        }
    }

    // texture transfer
    Components::TransferRequest request = {};
    request.cycle = false;
    request.type = Components::RequestType::UPLOAD_TO_TEXTURE;
    SDL_GPUTextureRegion& region = request.data.texture;
    region.texture = m_texture.Get();
    region.w = src_extent.x;
    region.h = src_extent.y;
    region.d = 1;
    region.x = dst_offset.x;
    region.y = dst_offset.y;
    request.p_src = static_cast<void*>(pixelRegion.data());

    m_p_render_system->UploadDataToBuffer({request});

    if (m_mipmaps) {
        m_p_render_system->GenerateMipMaps(m_texture);
    }
}

uint32_t Graphics::Texture2D::GetWidth() const {
    return m_width;
}

uint32_t Graphics::Texture2D::GetHeight() const {
    return m_height;
}

bool Graphics::Texture2D::HasMipMaps() const {
    return m_mipmaps;
}

SDL_GPUTextureSamplerBinding Graphics::Texture2D::GetBinding() {
    SDL_GPUTextureSamplerBinding binding = {};
    binding.texture = m_texture.Get();
    binding.sampler = m_p_sampler->Get();
    return binding;
}