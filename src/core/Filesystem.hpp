#pragma once

#include <string>

namespace Core {

    namespace Filesystem {

        //! Check if a file exists.
        bool FileExists(const std::string& filename);

        //! Load a human readable file as string.
        std::string LoadFileAsString(const std::string& filename);

        //! Create a directory if it doesn't exist.
        void CreateDirectory(const std::string& directory);

        //! Delete a path, whether it's a file or directory. If directory, will fail if not empty.
        void DeletePath(const std::string& path);
    };
}