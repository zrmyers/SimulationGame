#pragma once

#include <string>

namespace Core {

    class Engine;

    // Interface for Game.
    // Any game using the engine should implement this interface.
    class IGame {

        public:

            IGame() = default;
            IGame(const IGame& other) = delete;
            IGame(IGame&& other) = default;
            IGame& operator=(const IGame& other) = delete;
            IGame& operator=(IGame&& other) = default;
            virtual ~IGame() = default;

            // Game Update Loop
            virtual void Update(Core::Engine& engine) = 0;
    };
}