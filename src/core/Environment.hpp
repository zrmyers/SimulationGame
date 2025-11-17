#pragma once

#include <string>
#include <unordered_map>

namespace Core {

    //! Game Environment.
    class Environment {

        public:

            //! Value used when an environment variable could not be found.
            static const std::string ENV_NOT_FOUND;

            //! Initialize Environment.
            Environment(const std::list<const char*>& args);

            //! Get an environment variable.
            //!
            //! @returns the value of the environment variable. If not found, returns ENV_NOT_FOUND.
            const std::string& Get(const std::string& name) const;

        private:

            std::unordered_map<std::string, std::string> m_environment;
    };
}