#pragma once


namespace Core {

    class Engine;

    // Interface for Game.
    // Any game using the engine should implement this interface.
    class IGame {

        public:

            IGame(Engine& engine) : m_p_engine(&engine) {};
            IGame(const IGame& other) = delete;
            IGame(IGame&& other) = default;
            IGame& operator=(const IGame& other) = delete;
            IGame& operator=(IGame&& other) = default;
            virtual ~IGame() = default;

            virtual void Update() = 0;

            Engine& GetEngine() { return *m_p_engine;};

        private:

            Engine* m_p_engine;
    };
}