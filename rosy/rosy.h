#pragma once

#include <string>
#include <rosy/modules/timer/timer.h>

namespace rosy {
    struct Config {
        struct {
            std::string title = "ROSY";
            int width = 640;
            int height = 480;
        } window;
    };

    __attribute__((weak)) void conf(Config& c);
    __attribute__((weak)) void load();
    __attribute__((weak)) void unload();
    __attribute__((weak)) void draw();
    __attribute__((weak)) void focus(bool status);
    __attribute__((weak)) void update(timer::duration dt);
    __attribute__((weak)) void resize(int width, int height);
}