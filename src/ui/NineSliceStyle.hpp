#pragma once

#include "Element.hpp"
#include "graphics/Texture2D.hpp"

namespace UI {

    class NineSliceStyle {

        public:
            enum class Region : uint8_t {
                TOP_LEFT_CORNER = 0,
                TOP_RIGHT_CORNER,
                BOTTOM_LEFT_CORNER,
                BOTTOM_RIGHT_CORNER,
                TOP_EDGE,
                LEFT_EDGE,
                RIGHT_EDGE,
                BOTTOM_EDGE,
                CENTER
            };

            // number of textures that make up a nine-slice.
            static constexpr size_t SLICE_COUNT = 9U;

            static std::shared_ptr<NineSliceStyle> Load(Core::Engine& engine, const std::vector<std::string>& images);

            NineSliceStyle();

            void SetRegion(std::shared_ptr<Graphics::Texture2D> p_texture, Region region);
            const std::shared_ptr<Graphics::Texture2D>& GetRegion(Region region) const;

            float GetBorderWidth() const;
        private:
            float CalculateBorderWidth();

            std::vector<std::shared_ptr<Graphics::Texture2D>> m_textures;
            float m_border_width;
    };
}