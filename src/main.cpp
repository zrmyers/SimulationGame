#include "SimulationGame.hpp"
#include "core/Engine.hpp"
#include "core/Logger.hpp"
#include <exception>
#include <memory>
#include <span>

int main(int argc, const char** argv) {

    Core::Logger::SetLevel(Core::Logger::Level::INFO);

    std::list<const char*> args;

    for (int index = 1; index < argc; index++) {
        args.push_back(argv[index]); // NOLINT
    }

    try {

        Core::Engine engine(args);

        std::unique_ptr<SimulationGame> p_game = std::make_unique<SimulationGame>(engine);

        engine.SetGameInstance(std::move(p_game));
        engine.Run();

        Core::Logger::Info("Done Running. Shutdown.");

    } catch (Core::EngineException& error) {

        Core::Logger::Error("Caught Core::EngineException. Shutdown.");
        Core::Logger::Error(error.what());
    } catch (std::exception& error) {

        Core::Logger::Error("Unhandled exception.");
        Core::Logger::Error(error.what());
    }

}