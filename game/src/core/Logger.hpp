#pragma once
#include <string>

namespace Core {

    class Logger {

        public:

            // level value increases from highest to lowest priority.
            enum class Level : int32_t {

                //! Should not happen. If this is in log, error is likely fatal.
                ERROR = 0,

                //! Could indicate possible problems, but not enough to end the game.
                WARNING,

                //! Informational print. Important enough to put in log file.
                INFO,

                //! Trace level.
                TRACE
            };

            static void Write(Level level, const std::string& message);

            static void SetLevel(Level level);
            static Level GetLevel();

            static void Error(const std::string& message) {
                Write(Level::ERROR, message);
            };

            static void Warning(const std::string& message) {
                Write(Level::WARNING, message);
            };

            static void Info(const std::string& message) {
                Write(Level::INFO, message);
            };

            static void Trace(const std::string& message) {
                Write(Level::TRACE, message);
            };

        private:

            static const char* LevelToString(Level level);

            // Current log level.
            static Level s_level;
    };

}