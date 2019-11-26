#include "timer.h"
#include "../../module.h"

#include <thread>

namespace rosy::chrono {
    namespace {
        module<Timer> instance;
    }

    void Timer::reset() {
        m_step = duration(0);
        m_time = clock::now();
    }

    duration Timer::step() {
        m_frames++;
        m_last = m_time;

        m_time = clock::now();
        m_step = m_time - m_last;

        double time_since_last = std::chrono::duration<double>(m_time - m_prev_fps_update).count();
        if (time_since_last > 1.0) {
            m_fps = size_t((m_frames / time_since_last) + 0.5);
            m_delta = time_since_last / m_frames;
            m_prev_fps_update = m_time;
            m_frames = 0;
        }
        return m_step;
    }

    point Timer::getTime() {
        return clock::now();
    }

    size_t Timer::getFPS() {
        return m_fps;
    }

    double Timer::getAverageDelta() {
        return m_delta;
    }

    void reset() {
        instance->reset();
    }
    duration step() {
        return instance->step();
    }
    point getTime() {
        return instance->getTime();
    }
    size_t getFPS() {
        return instance->getFPS();
    }
}