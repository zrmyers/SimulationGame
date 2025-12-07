#pragma once

#include "components/Renderable.hpp"
#include "core/Engine.hpp"
#include "core/IGame.hpp"
#include "ecs/ECS.hpp"
#include "graphics/Font.hpp"
#include "sdl/TTF.hpp"

class SimulationGame : public Core::IGame {

    public:

        explicit SimulationGame(Core::Engine& engine);

        void Update() override;

    private:

        void InitializeGUI();

        std::shared_ptr<Graphics::Font> m_p_font;
        ECS::Entity m_text_entity;

        ECS::Entity m_camera_entity;

        float m_rotateAngle{0.0F};

        ECS::Entity m_gui_entity;

};