#pragma once

#include <string>

namespace Core {

    namespace Filesystem {

        //! Load a human readable file as string.
        std::string LoadFileAsString(const std::string& filename);
    };
}