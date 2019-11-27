#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <rosy.h>
#include "window.h"
#include "module.h"
#include "graphics/graphics.h"

namespace {
    module<rosy::graphics::Graphics> m_graphics;
    module<rosy::window::Window> instance;
}

namespace rosy::window {
    Window::Window() {
        SDL_InitSubSystem(SDL_INIT_VIDEO);
    }

    Window::~Window() {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    bool Window::setWindow(const std::string& title, const int width, const int height) {
        m_width = width;
        m_height = height;

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        m_window = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if (m_window == nullptr) {
            return false;
        }
        m_context = SDL_GL_CreateContext(m_window);
        if (m_context == nullptr) {
            return false;
        }

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetSwapInterval(0);

        SDL_SetWindowResizable(m_window, SDL_TRUE);

        glewInit();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        return true;
    }

    void Window::destroy() {
        SDL_GL_DeleteContext(m_context);
        SDL_DestroyWindow(m_window);
        m_context = nullptr;
        m_window = nullptr;
    }

    void Window::swapBuffers() {
        SDL_GL_SwapWindow(m_window);
    }

    void Window::wrapMouse() {
        SDL_WarpMouseInWindow(m_window, m_width / 2, m_height / 2);
    }

    void Window::setRelativeMouse(bool relative) {
//        if (is_mouse_relative != relative) {
            is_mouse_relative = relative;

            if (relative) {
                SDL_ShowCursor(SDL_DISABLE);
                SDL_WarpMouseInWindow(m_window, m_width / 2, m_height / 2);
            } else {
                SDL_ShowCursor(SDL_ENABLE);
            }
    }

    int getWidth() {
        return instance->getWidth();
    }

    int getHeight() {
        return instance->getHeight();
    }

    void setRelativeMouse(bool relative) {
        instance->setRelativeMouse(relative);
    }
}