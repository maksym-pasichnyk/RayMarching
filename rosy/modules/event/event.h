#pragma once

#include <SDL2/SDL_events.h>

namespace rosy::event {
    using Event = SDL_Event;

    inline bool poll(Event& event) {
        return SDL_PollEvent(&event);
    }

    inline void pump() {
        SDL_PumpEvents();
    }
}