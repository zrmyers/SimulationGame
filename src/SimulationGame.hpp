#pragma once

#include "core/Engine.hpp"
#include "core/IGame.hpp"
#include "graphics/Renderable.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/TextRenderer.hpp"
#include "sdl/SDL.hpp"
#include "sdl/TTF.hpp"

class SimulationGame : public Core::IGame {

    public:

        explicit SimulationGame(Core::Engine& engine);

        void Update() override;

    private:

        SDL::TTF::Font m_font;
        Graphics::TextComponent m_text;

        //! Uniforms
        Graphics::UniformData m_uniform;

};