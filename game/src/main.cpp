#include "core/Engine.hpp"
#include "core/Logger.hpp"

int main(int argc, const char** argv) {

    Core::Logger::SetLevel(Core::Logger::Level::INFO);

    try {

        Core::Engine engine(argc, argv);

        engine.Run();

        Core::Logger::Info("Done Running. Shutdown.");

    } catch (Core::EngineException& error) {

        Core::Logger::Error("Caught Core::EngineException. Shutdown.");
        Core::Logger::Error(error.what());
    }

}