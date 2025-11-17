#include "core/Engine.hpp"
#include "core/Logger.hpp"
#include <span>

int main(int argc, const char** argv) {

    Core::Logger::SetLevel(Core::Logger::Level::INFO);

    std::list<const char*> args;

    for (int index = 1; index < argc; index++) {
        args.push_back(argv[index]); // NOLINT
    }

    try {

        Core::Engine engine(args);

        engine.Run();

        Core::Logger::Info("Done Running. Shutdown.");

    } catch (Core::EngineException& error) {

        Core::Logger::Error("Caught Core::EngineException. Shutdown.");
        Core::Logger::Error(error.what());
    }

}