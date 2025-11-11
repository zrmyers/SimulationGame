#include "SDL.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>


SDL::Error::Error(const std::string& msg)
    : m_msg(msg + SDL_GetError()) {
}

const char * SDL::Error::what() const {
    return m_msg.c_str();
}


SDL::Context::Context(SDL_InitFlags flags) {
    if(!SDL_Init(flags)) {
        throw Error("SDL_Init() failed: ");
    }
}

SDL::Context::~Context() {
    SDL_Quit();
}