#pragma once

#include "SDL.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3_ttf/SDL_textengine.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdint>
#include <string>

namespace SDL::TTF {

    class Context {

        public:
            Context();
            Context(const Context& other) = delete;
            Context(Context&& other) = delete;
            Context& operator=(const Context& other) = delete;
            Context& operator=(Context&& other) = delete;
            ~Context();
    };

    class Font {

        public:
            Font();
            Font(const std::string& font_filename, float ptsize);
            Font(const Font& other) = delete;
            Font(Font&& other) noexcept;
            Font& operator=(const Font& other) = delete;
            Font& operator=(Font&& other) noexcept;
            ~Font();

            //! Get internal font pointer.
            TTF_Font* Get();

            //! Used to enable or disable signed distance field.
            void SetSDF(bool use_sdf);
            bool GetSDF() const;

            //! Used to configure alignment when rendering horizontal text.
            void SetHorizontalAlignment(TTF_HorizontalAlignment wrap_alignment);

        private:

            TTF_Font* m_p_font;
    };

    class TextEngine {

        public:
            TextEngine();
            TextEngine(GpuDevice& device);
            TextEngine(const TextEngine& other) = delete;
            TextEngine(TextEngine&& other) noexcept;
            TextEngine& operator=(const TextEngine& other) = delete;
            TextEngine& operator=(TextEngine&& other) noexcept;
            ~TextEngine();

            TTF_TextEngine* Get();

        private:

            TTF_TextEngine* m_p_engine;
    };

    class Text {

        public:

            Text();
            Text(TextEngine& engine, Font& font, const std::string& str);
            Text(const Text& other) = delete;
            Text(Text&& other) noexcept;
            Text& operator=(const Text& other) = delete;
            Text& operator=(Text&& other) noexcept;
            ~Text();

            //! Set the string that is displayed for the text object.
            void SetString(const std::string& str);

            //! Get the size of the text.
            void GetSize(int& width, int& height);

            //! Get the position of the text.
            void GetPosition(int& pos_x, int& pos_y);

            //! Get the text draw data.
            TTF_GPUAtlasDrawSequence* GetGpuDrawData();

        private:

            TTF_Text* m_p_text;
    };
}