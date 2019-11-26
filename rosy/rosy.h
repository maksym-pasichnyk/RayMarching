#pragma once

#include <string>

namespace rosy {
    struct Config {
        struct {
            std::string title = "ROSY";
            int width = 640;
            int height = 480;
        } window;
    };

    __attribute__((weak)) void conf(Config& c);
}