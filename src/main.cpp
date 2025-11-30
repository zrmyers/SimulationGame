#include "SimulationGame.hpp"
#include "components/Camera.hpp"
#include "components/Renderable.hpp"
#include "components/Text.hpp"
#include "components/Transform.hpp"
#include "components/Sprite.hpp"
#include "core/Engine.hpp"
#include "core/Logger.hpp"
#include "ecs/ECS.hpp"
#include "systems/GuiSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/SpriteSystem.hpp"
#include "systems/TextSystem.hpp"
#include <exception>
#include <memory>


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
        registry.RegisterComponent<Components::Sprite>();

        // register systems
        registry.RegisterSystem(std::make_unique<Systems::RenderSystem>(engine));
        registry.RegisterSystem(std::make_unique<Systems::TextSystem>(engine));
        registry.RegisterSystem(std::make_unique<Systems::SpriteSystem>(engine));
        registry.RegisterSystem(std::make_unique<Systems::GuiSystem>(engine));

        // setup component registration
        registry.SetSystemSignature<Systems::RenderSystem>(
            registry.GetComponentSignature<Components::Renderable>()
        );
        registry.SetSystemSignature<Systems::TextSystem>(
            registry.GetComponentSignature<Components::Text>()
            | registry.GetComponentSignature<Components::Transform>()
        );
        registry.SetSystemSignature<Systems::SpriteSystem>(
            registry.GetComponentSignature<Components::Sprite>()
            | registry.GetComponentSignature<Components::Transform>()
        );

        // setup dependencies
        registry.SetSystemDependency<Systems::RenderSystem, Systems::TextSystem>();
        registry.SetSystemDependency<Systems::RenderSystem, Systems::SpriteSystem>();
        registry.SetSystemDependency<Systems::TextSystem, Systems::GuiSystem>();
        registry.SetSystemDependency<Systems::SpriteSystem, Systems::GuiSystem>();

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