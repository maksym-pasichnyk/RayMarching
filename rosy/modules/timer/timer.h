#pragma once

#include <chrono>
#include <thread>

namespace rosy::timer {
    using clock = std::chrono::high_resolution_clock;
    using point = clock::time_point;
    using duration = std::chrono::duration<long, std::nano>;

    struct Timer {
        void reset();
        duration step();
        point getTime();
        size_t getFPS();
        double getAverageDelta();

    private:
        point m_time = clock::now();
        point m_prev_fps_update = m_time;
        point m_last = m_time;
        duration m_step = duration(0);
        size_t m_frames;
        size_t m_fps;
        double m_delta;
    };
    template<typename Rep, typename Period>
    inline void sleep(const std::chrono::duration<Rep, Period>& time) {
        std::this_thread::sleep_for(time);
    }

    point getTime();
    size_t getFPS();
}