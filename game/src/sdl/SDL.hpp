
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <exception>
#include <string>

// Light-weight RAII wrapper for SDL library.
namespace SDL {

    class Error : public std::exception {

        public:
            Error(const std::string& msg);

            const char * what() const override;

        private:

            std::string m_msg;
    };

    class Context {

        public:
            Context(SDL_InitFlags flags);
            Context(const Context& other) = delete;
            Context(Context&& other) = default;
            Context& operator=(const Context& other) = delete;
            Context& operator=(Context&& other) = default;
            ~Context();

    };

    class Window {

        public:
            Window(const char* title, int width, int height, SDL_WindowFlags flags);
            Window(const Window& other) = delete;
            Window(Window&& other) = default;
            Window& operator=(const Window& other) = delete;
            Window& operator=(Window&& other) = default;
            ~Window();


    }
}