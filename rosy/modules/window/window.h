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

        // todo: move to mouse module
        void setRelativeMouse(bool relative);

        inline void setSize(const int width, const int height) {
            m_width = width;
            m_height = height;
        }

        inline void setFocus(const bool focus) {
            m_focus = focus;

            if (is_mouse_relative) {
                if (focus) {
                    SDL_ShowCursor(SDL_DISABLE);
                    SDL_WarpMouseInWindow(m_window, m_width / 2, m_height / 2);
                } else {
                    SDL_ShowCursor(SDL_ENABLE);
                }
            }
        }

        inline bool hasFocus() {
            return m_focus;
        }

        inline int getWidth() const {
            return m_width;
        }

        inline int getHeight() const {
            return m_height;
        }

        // todo: move to mouse module
        inline bool isMouseRelative() const {
            return is_mouse_relative;
        }
    private:
        int m_width;
        int m_height;
        bool m_focus = false;
        bool is_mouse_relative = false;
        SDL_Window *m_window = nullptr;
        SDL_GLContext m_context = nullptr;
    };

    int getWidth();
    int getHeight();

    // todo: move to mouse module
    void setRelativeMouse(bool relative);
}