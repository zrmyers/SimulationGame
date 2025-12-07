#pragma once

#include "core/Engine.hpp"
#include "sdl/TTF.hpp"
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>

namespace Systems {
    class TextSystem;
}

namespace Graphics {

    class Font {

        public:

            Font(
                Core::AssetLoader& assetLoader,
                Systems::TextSystem& textSystem,
                const std::string& name,
                float pt_size,
                bool use_sdf,
                TTF_HorizontalAlignment alignment);

            std::shared_ptr<SDL::TTF::Text> CreateText(const std::string& text_str);

            bool GetSDF() const;

        private:

            Systems::TextSystem* m_p_text_system {nullptr};
            SDL::TTF::Font m_font;
    };
}