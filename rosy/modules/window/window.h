#pragma once

#include <string>
#include <SDL2/SDL.h>

namespace rosy::window {
    struct Window {
    public:
        Window();
        ~Window();

        bool setWindow(const std::string& title, const int width, const int height);
        void destroy();
        void swapBuffers();
        void wrapMouse();

    private:
        int m_width;
        int m_height;
        SDL_Window *m_window = nullptr;
        SDL_GLContext m_context = nullptr;
    };
}