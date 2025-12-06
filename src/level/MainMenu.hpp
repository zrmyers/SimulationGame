#pragma once

#include "Level.hpp"
#include "core/Engine.hpp"
#include "ecs/ECS.hpp"
#include "sdl/TTF.hpp"
#include "ui/UI.hpp"

namespace Level {
    class MainMenu : public Level {

        public:

            //! Constructor for the main menu.
            MainMenu(Core::Engine& engine, std::string name);

            //! Load resources for the level.
            void Load() override;

            //! Update processing for the level.
            void Update() override;

            //! Unload resources for the level.
            void Unload() override;

        private:

            //! primary entity to which main menu is attached.
            ECS::Entity m_menu_entity;

            //! Menu background style.
            UI::NineSliceStyle m_background_style;

            //! Button style.
            UI::NineSliceStyle m_button_style;

            //! Menu font.
            std::shared_ptr<SDL::TTF::Font> m_p_font;


    };
}