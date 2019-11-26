#include <rosy/modules/graphics/camera.h>
#include "module.h"
#include "modules/timer/timer.h"
#include "modules/window/window.h"
#include "modules/graphics/graphics.h"
#include "modules/event/event.h"
#include "modules/input/input.h"
#include "rosy.h"

namespace {
    module<rosy::graphics::Graphics> m_graphics;
    module<rosy::graphics::Camera> m_camera;
    module<rosy::window::Window> m_window;
    module<rosy::chrono::Timer> m_timer;
    module<rosy::input::Input> m_input;
}

namespace rosy {
    extern "C" int main(int argc, char **argv) {
        using std::chrono::duration;
        using std::chrono::milliseconds;
        using rosy::chrono::sleep;

        auto start = m_timer->getTime();
        m_timer->reset();

        Config c{};
        if (conf) {
            conf(c);
        }

        m_window->setWindow(c.window.title, c.window.width, c.window.height);

        m_camera->transform.position = {0, 1, 10};

        auto shader = m_graphics->shader;
        shader.fragment.set("camera.position", m_camera->transform.position.get());
        shader.fragment.set("camera.localToWorldMatrix", m_camera->transform.localToWorldMatrix());
        shader.fragment.set("camera.cameraToWorldMatrix", m_camera->cameraToWorldMatrix());

        m_timer->step();
        for (bool running = true; running;) {
            rosy::event::Event event;
            while (rosy::event::poll(event)) {
                if (event.type == SDL_WINDOWEVENT) {
                    if (event.window.event == SDL_WINDOWEVENT_TAKE_FOCUS) {
                        SDL_ShowCursor(SDL_DISABLE);
                        m_window->wrapMouse();
                    }
                    continue;
                }
                if (event.type == SDL_QUIT) {
                    running = false;
                    continue;
                }
                if (event.type == SDL_MOUSEMOTION) {
                    m_window->wrapMouse();
                    m_input->mouseMove(event.motion);
                    continue;
                }
            }
            auto dt = m_timer->step();

            m_input->update();
            m_camera->update(dt);

            auto iTime = duration<double>(m_timer->getTime() - start);
            shader.fragment.setFloat("iTime", iTime.count());

            m_graphics->clear();
            m_graphics->draw();
            m_graphics->present();

            sleep(milliseconds(1));
        }
        m_window->destroy();
        return 0;
    }
}