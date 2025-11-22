#pragma once

#include "core/Engine.hpp"
#include "core/IGame.hpp"
class SimulationGame : public Core::IGame {

    public:

        explicit SimulationGame(Core::Engine& engine);

        void Update(Core::Engine& engine) override;

};