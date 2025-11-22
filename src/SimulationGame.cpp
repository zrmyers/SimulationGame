#include "SimulationGame.hpp"
#include "core/Logger.hpp"
#include <string>


SimulationGame::SimulationGame(Core::Engine& engine) {
    Core::Logger::Info("Initializing Game.");
}

void SimulationGame::Update(Core::Engine& engine) {
    //Core::Logger::Info("FPS: " + std::to_string(1.0F/ engine.GetDeltaTimeSec()));
}