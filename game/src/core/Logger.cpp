#include "Logger.hpp"
#include <cstdint>
#include <iostream>
#include <string>
#include "Engine.hpp"

Core::Logger::Level Core::Logger::s_level = Core::Logger::Level::INFO;

void Core::Logger::Write(Level level, const std::string& message) {

    if (static_cast<int32_t>(level) <= static_cast<int32_t>(s_level)) {

        std::cout << std::string(LevelToString(level)) << ": " << message << std::endl;
    }
}

void Core::Logger::SetLevel(Level level) {

    s_level = level;
}

Core::Logger::Level Core::Logger::GetLevel() {
    return s_level;
}


const char* Core::Logger::LevelToString(Level level) {

    switch (level) {
        case Level::ERROR:
            return "ERROR";
            break;

        case Level::WARNING:
            return "WARNING";
            break;

        case Level::INFO:
            return "INFO";
            break;

        case Level::TRACE:
            return "TRACE";
            break;

        default:
            break;
    }

    return "UNKNOWN";
}