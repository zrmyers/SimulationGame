#include "SimulationGame.hpp"
#include "components/Camera.hpp"
#include "components/Renderable.hpp"
#include "components/Text.hpp"
#include "components/Transform.hpp"
#include "core/Engine.hpp"
#include "core/Logger.hpp"
#include "ecs/ECS.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/TextSystem.hpp"
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
        ECS::Registry& registry = engine.GetEcsRegistry();

        // register components
        registry.RegisterComponent<Components::Camera>();
        registry.RegisterComponent<Components::Text>();
        registry.RegisterComponent<Components::Renderable>();
        registry.RegisterComponent<Components::Transform>();

        // register systems
        registry.RegisterSystem(std::make_unique<Systems::RenderSystem>(engine));
        registry.RegisterSystem(std::make_unique<Systems::TextSystem>(engine));

        // setup component registration
        registry.SetSystemSignature<Systems::RenderSystem>(
            registry.GetComponentSignature<Components::Renderable>()
        );
        registry.SetSystemSignature<Systems::TextSystem>(
            registry.GetComponentSignature<Components::Text>()
            | registry.GetComponentSignature<Components::Transform>()
        );

        // setup dependencies
        registry.SetSystemDependency<Systems::RenderSystem, Systems::TextSystem>();

        // instantiate the game.
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