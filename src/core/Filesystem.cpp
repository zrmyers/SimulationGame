#include "Filesystem.hpp"

#include <fstream>
#include <sstream>

std::string Core::Filesystem::LoadFileAsString(const std::string& filename) {
    std::string filedata;
    std::ifstream file(filename, std::ios_base::binary);
    if (file.is_open()) {
        std::stringstream filedatastream;
        filedatastream << file.rdbuf();
        filedata = filedatastream.str();
    }

    return filedata;
}