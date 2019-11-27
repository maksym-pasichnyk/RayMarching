#include "module.h"
#include "modules/timer/timer.h"
#include "modules/window/window.h"
#include "modules/graphics/graphics.h"
#include "modules/event/event.h"
#include "modules/input/input.h"
#include "rosy.h"

namespace {
    module<rosy::graphics::Graphics> m_graphics;
    module<rosy::window::Window> m_window;
    module<rosy::timer::Timer> m_timer;
    module<rosy::input::Input> m_input;
}

namespace rosy {
    extern "C" int main(int argc, char **argv) {
        using std::chrono::duration;
        using std::chrono::milliseconds;
        using rosy::timer::sleep;

        m_timer->reset();

        Config c{};
        if (conf) {
            conf(c);
        }

        m_window->setWindow(c.window.title, c.window.width, c.window.height);

        if (load) {
            load();
        }

        m_timer->step();
        for (bool running = true; running;) {
            rosy::event::Event event;
            while (rosy::event::poll(event)) {
                if (event.type == SDL_WINDOWEVENT) {
                    auto data = event.window;
                    if (data.event == SDL_WINDOWEVENT_TAKE_FOCUS) {
                        m_window->setFocus(true);
                        if (m_window->isMouseRelative()) {
                            m_input->resetMouse(
                                m_window->getWidth() >> 1,
                                m_window->getHeight() >> 1
                            );
                        }
                        if (focus) focus(true);
                        continue;
                    }
                    if (data.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                        m_window->setFocus(false);
                        if (focus) focus(false);
                        continue;
                    }
                    if (data.event == SDL_WINDOWEVENT_RESIZED) {
                        m_window->setSize(data.data1, data.data2);
                        if (resize) resize(data.data1, data.data2);
                        continue;
                    }
                    continue;
                }
                if (event.type == SDL_QUIT) {
                    running = false;
                    continue;
                }
                if (event.type == SDL_MOUSEMOTION) {
                    if (m_window->hasFocus()) {
                        // todo: move to mouse module
                        if (m_window->isMouseRelative()) {
                            m_window->wrapMouse();
                        }
                        m_input->mouseMove(event.motion);
                    }
                    continue;
                }
            }
            auto dt = m_timer->step();
            m_input->update();

            if (update) {
                update(dt);
            }

            m_graphics->clear();
            if (draw) {
                draw();
            }
            m_graphics->present();

            sleep(milliseconds(1));
        }

        if (unload) {
            unload();
        }

        m_window->destroy();
        return 0;
    }
}