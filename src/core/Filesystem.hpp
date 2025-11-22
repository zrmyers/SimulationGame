#pragma once

#include <string>

namespace Core {

    namespace Filesystem {

        //! Check if a file exists.
        bool FileExists(const std::string& filename);

        //! Load a human readable file as string.
        std::string LoadFileAsString(const std::string& filename);
    };
}