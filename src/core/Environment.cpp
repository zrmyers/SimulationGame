#include "Environment.hpp"
#include "Engine.hpp"
#include "Logger.hpp"

#include <sstream>

const std::string Core::Environment::ENV_NOT_FOUND;

Core::Environment::Environment(const std::list<const char*>& args) {
    // expect that arguments are provided in key=value format.
    for (const char* arg : args) {

        std::string argument = std::string(arg);

        size_t splitPos = argument.find('=');
        if (splitPos > 0 && splitPos < argument.length() - 1) {
            std::string key = argument.substr(0, splitPos);

            m_environment[key] = argument.substr(splitPos + 1);

            Core::Logger::Info(key + " = " + m_environment[key]);
        }
        else {

            std::stringstream msg;
            msg << "Failed to parse argument from command line, " << argument << "\n"
                << "invalid variable. expect format <key>=<value>" << "\n";

            throw EngineException(msg.str());
        }
    }
}

const std::string& Core::Environment::Get(const std::string& name) const {

    auto valueIter = m_environment.find(name);
    if (valueIter != m_environment.end()) {
        return valueIter->second;
    }

    return ENV_NOT_FOUND;
}
