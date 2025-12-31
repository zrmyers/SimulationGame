#pragma once

#include <functional>
#include <unordered_map>
#include <SDL3/SDL_events.h>
#include <glm/vec2.hpp>

namespace Components {

    using EventCallback_t = std::function<void(const SDL_Event&)>;
    struct InputHandler {

        public:

            //! Which events are being listened for by handler.
            std::unordered_map<SDL_EventType, bool> m_events;

            //! Callback function for handling event.
            EventCallback_t m_callback;

            //! Bounds to use for collision tracking for mouse event. If no bounds are specified, the event is ignored.
            //!
            //! Coordinates are in pixels relative to top-left corner of screen, where 0,0 is top left corner, and ResX,ResY is
            //! bottom right corner.
            glm::vec2 m_top_left;
            glm::vec2 m_bottom_right;
    };
}