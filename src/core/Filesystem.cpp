#include "Filesystem.hpp"
#include "sdl/SDL.hpp"

#include <SDL3/SDL_filesystem.h>
#include <fstream>
#include <sstream>


bool Core::Filesystem::FileExists(const std::string& filename) {
    bool exists = false;
    std::ifstream file(filename, std::ios_base::binary);
    exists = file.is_open();
    return exists;
}

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


void Core::Filesystem::CreateDirectory(const std::string& directory) {

    if(!SDL_CreateDirectory(directory.c_str())) {
        throw SDL::Error("Failed to create directory: " + directory);
    }
}

void Core::Filesystem::DeletePath(const std::string& path) {
    if(!SDL_RemovePath(path.c_str())) {
        throw SDL::Error("Failed to delete path: " + path);
    }
}