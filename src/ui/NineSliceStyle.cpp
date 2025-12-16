#include "NineSliceStyle.hpp"

//----------------------------------------------------------------------------------------------------------------------
// Nine Slice Style

std::shared_ptr<UI::NineSliceStyle> UI::NineSliceStyle::Load(Core::Engine& engine, const std::string& imageName, int32_t borderWidth) {

    Core::AssetLoader& assetLoader = engine.GetAssetLoader();
    UI::NineSliceStyle style;

    style.m_textures.resize(SLICE_COUNT);

    Systems::RenderSystem& renderSystem = engine.GetEcsRegistry().GetSystem<Systems::RenderSystem>();
    std::shared_ptr<SDL::GpuSampler> p_cornerSampler;
    std::shared_ptr<SDL::GpuSampler> p_horizontalSampler;
    std::shared_ptr<SDL::GpuSampler> p_verticalSampler;
    std::shared_ptr<SDL::GpuSampler> p_fillSampler;

    // Create samplers
    SDL_GPUSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    samplerCreateInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerCreateInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.enable_anisotropy = false;
    samplerCreateInfo.max_anisotropy = 0; // NOLINT

    p_cornerSampler = std::make_shared<SDL::GpuSampler>(renderSystem.CreateSampler(samplerCreateInfo));
    samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    p_horizontalSampler = std::make_shared<SDL::GpuSampler>(renderSystem.CreateSampler(samplerCreateInfo));
    samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    p_verticalSampler = std::make_shared<SDL::GpuSampler>(renderSystem.CreateSampler(samplerCreateInfo));
    samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    p_fillSampler = std::make_shared<SDL::GpuSampler>(renderSystem.CreateSampler(samplerCreateInfo));

    // load image
    SDL::Image image(assetLoader.GetImageDir() + "/" + imageName);

    // instantiate textures
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::TOP_LEFT_CORNER)]
        = std::make_shared<Graphics::Texture2D>(
            engine, p_cornerSampler, borderWidth, borderWidth, false);
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::TOP_RIGHT_CORNER)]
        = std::make_shared<Graphics::Texture2D>(
            engine, p_cornerSampler, borderWidth, borderWidth, false);
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::BOTTOM_LEFT_CORNER)]
        = std::make_shared<Graphics::Texture2D>(
            engine, p_cornerSampler, borderWidth, borderWidth, false);
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::BOTTOM_RIGHT_CORNER)]
        = std::make_shared<Graphics::Texture2D>(
            engine, p_cornerSampler, borderWidth, borderWidth, false);
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::TOP_EDGE)]
        = std::make_shared<Graphics::Texture2D>(
            engine, p_horizontalSampler, image.GetWidth() - (2 * borderWidth), borderWidth, false);
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::BOTTOM_EDGE)]
        = std::make_shared<Graphics::Texture2D>(
            engine, p_horizontalSampler, image.GetWidth() - (2 * borderWidth), borderWidth, false);
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::LEFT_EDGE)]
        = std::make_shared<Graphics::Texture2D>(
            engine, p_verticalSampler, borderWidth, image.GetHeight() - (2 * borderWidth), false);
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::RIGHT_EDGE)]
        = std::make_shared<Graphics::Texture2D>(
            engine, p_verticalSampler, borderWidth, image.GetHeight() - (2 * borderWidth), false);
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::CENTER)]
        = std::make_shared<Graphics::Texture2D>(
            engine, p_fillSampler, image.GetWidth() - (2 * borderWidth), image.GetHeight() - (2 * borderWidth), false);

    // load image data
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::TOP_LEFT_CORNER)]
        ->LoadImageData(image, {0, 0}, {borderWidth, borderWidth});
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::TOP_RIGHT_CORNER)]
        ->LoadImageData(image, {image.GetWidth() - borderWidth, 0}, {borderWidth, borderWidth});
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::BOTTOM_LEFT_CORNER)]
        ->LoadImageData(image, {0, image.GetHeight() - borderWidth}, {borderWidth, borderWidth});
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::BOTTOM_RIGHT_CORNER)]
        ->LoadImageData(image, {image.GetWidth() - borderWidth, image.GetHeight() - borderWidth}, {borderWidth, borderWidth});
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::TOP_EDGE)]
        ->LoadImageData(image, { borderWidth, 0}, {image.GetWidth() - (2* borderWidth), borderWidth});
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::BOTTOM_EDGE)]
        ->LoadImageData(image, { borderWidth, image.GetHeight() - borderWidth}, {image.GetWidth() - (2* borderWidth), borderWidth});
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::LEFT_EDGE)]
        ->LoadImageData(image, { 0, borderWidth}, {borderWidth, image.GetHeight() - (2*borderWidth)});
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::RIGHT_EDGE)]
        ->LoadImageData(image, { image.GetWidth() - borderWidth, borderWidth}, {borderWidth, image.GetHeight() - (2*borderWidth)});
    style.m_textures[static_cast<size_t>(NineSliceStyle::Region::CENTER)]
        ->LoadImageData(image, { borderWidth, borderWidth}, {image.GetWidth() - (2* borderWidth), image.GetHeight() - (2*borderWidth)});

    style.m_border_width = static_cast<float>(borderWidth);

    return std::make_shared<NineSliceStyle>(std::move(style));
}

std::shared_ptr<UI::NineSliceStyle> UI::NineSliceStyle::Load(Core::Engine& engine, const std::vector<std::string>& images) {

    Core::AssetLoader& assetLoader = engine.GetAssetLoader();
    UI::NineSliceStyle style;

    if (images.size() != NineSliceStyle::SLICE_COUNT) {
        throw Core::EngineException("Not enough data to initialize nine-slice. Need 9 image files to work.");
    }

    style.m_textures.resize(images.size());

    Systems::RenderSystem& renderSystem = engine.GetEcsRegistry().GetSystem<Systems::RenderSystem>();
    std::shared_ptr<SDL::GpuSampler> p_cornerSampler;
    std::shared_ptr<SDL::GpuSampler> p_horizontalSampler;
    std::shared_ptr<SDL::GpuSampler> p_verticalSampler;
    std::shared_ptr<SDL::GpuSampler> p_fillSampler;

    // Create samplers
    SDL_GPUSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    samplerCreateInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerCreateInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.enable_anisotropy = true;
    samplerCreateInfo.max_anisotropy = 16; // NOLINT

    p_cornerSampler = std::make_shared<SDL::GpuSampler>(renderSystem.CreateSampler(samplerCreateInfo));
    samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    p_horizontalSampler = std::make_shared<SDL::GpuSampler>(renderSystem.CreateSampler(samplerCreateInfo));
    samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    p_verticalSampler = std::make_shared<SDL::GpuSampler>(renderSystem.CreateSampler(samplerCreateInfo));
    samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    p_fillSampler = std::make_shared<SDL::GpuSampler>(renderSystem.CreateSampler(samplerCreateInfo));

    // now create images
    for (size_t index = 0; index < images.size(); index++) {

        Region region = static_cast<Region>(index);
        SDL::Image image(assetLoader.GetImageDir() + "/" + images.at(index));
        std::shared_ptr<Graphics::Texture2D> p_texture = nullptr;
        switch (region) {
            case Region::TOP_LEFT_CORNER:
            case Region::BOTTOM_LEFT_CORNER:
            case Region::TOP_RIGHT_CORNER:
            case Region::BOTTOM_RIGHT_CORNER:
                p_texture = std::make_shared<Graphics::Texture2D>(
                    engine, p_cornerSampler, image.GetWidth(), image.GetHeight(), false);
                break;

            case Region::LEFT_EDGE:
            case Region::RIGHT_EDGE:
                p_texture = std::make_shared<Graphics::Texture2D>(
                    engine, p_verticalSampler, image.GetWidth(), image.GetHeight(), false);
                break;

            case Region::TOP_EDGE:
            case Region::BOTTOM_EDGE:
                p_texture = std::make_shared<Graphics::Texture2D>(
                    engine, p_horizontalSampler, image.GetWidth(), image.GetHeight(), false);
                break;

            case Region::CENTER:
                p_texture = std::make_shared<Graphics::Texture2D>(
                    engine, p_fillSampler, image.GetWidth(), image.GetHeight(), false);
                break;

            default:
                break;
        }

        p_texture->LoadImageData( image);

        style.m_textures[index] = std::move(p_texture);
    }

    style.m_border_width = style.CalculateBorderWidth();

    return std::make_shared<NineSliceStyle>(std::move(style));
}

UI::NineSliceStyle::NineSliceStyle()
    : m_border_width(0.0F) {
    m_textures.resize(NineSliceStyle::SLICE_COUNT);
}

void UI::NineSliceStyle::SetRegion(std::shared_ptr<Graphics::Texture2D> p_texture, Region region) {

    size_t index = static_cast<size_t>(region);

    m_textures.at(index) = std::move(p_texture);
}

const std::shared_ptr<Graphics::Texture2D>& UI::NineSliceStyle::GetRegion(Region region) const {

    size_t index = static_cast<size_t>(region);

    return m_textures.at(index);
}

float UI::NineSliceStyle::GetBorderWidth() const {
    return m_border_width;
}

float UI::NineSliceStyle::CalculateBorderWidth() {

    uint32_t minBorderWidth = 0;

    for (size_t index = 0; index < m_textures.size(); index++) {

        std::shared_ptr<Graphics::Texture2D>& p_texture = m_textures.at(index);
        Region region = static_cast<Region>(index);

        switch (region) {

            case Region::TOP_LEFT_CORNER:
            case Region::BOTTOM_LEFT_CORNER:
            case Region::TOP_RIGHT_CORNER:
            case Region::BOTTOM_RIGHT_CORNER:
                minBorderWidth = std::max({minBorderWidth, p_texture->GetWidth(), p_texture->GetHeight()});
                break;

            case Region::LEFT_EDGE:
            case Region::RIGHT_EDGE:
                minBorderWidth = std::max(minBorderWidth, p_texture->GetWidth());
                break;

            case Region::TOP_EDGE:
            case Region::BOTTOM_EDGE:
                minBorderWidth = std::max(minBorderWidth, p_texture->GetHeight());
                break;

            case Region::CENTER:
            default:
                break;
        }
    }
    return static_cast<float>(minBorderWidth);
}