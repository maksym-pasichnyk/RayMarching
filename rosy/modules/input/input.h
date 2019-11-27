#pragma once

#include "module.h"

#include <glm/vec2.hpp>
#include <SDL2/SDL_events.h>

#include <cstring>

namespace rosy::input {
    struct Input {
        Input() {
            m_current_state.fill(0);
        }

        void update() {
            m_moved = m_position != m_previous;
            if (m_moved) {
                m_delta = m_position - m_previous;
                m_previous = m_position;
            }

            memcpy(&m_previous_state, &m_current_state, sizeof(m_previous_state));
            memcpy(&m_current_state, m_keys, sizeof(m_current_state));
        }

        inline glm::ivec2 getMousePosition() const noexcept {
            return m_position;
        }

        inline glm::ivec2 getMouseDelta() const noexcept {
            return m_delta;
        }

        inline void resetMouse(const int x, const int y) noexcept {
            m_moved = false;
            m_position.x = x;
            m_position.y = y;
            m_previous.x = x;
            m_previous.y = y;
            m_delta.x = 0;
            m_delta.y = 0;
        }

        inline void mouseMove(const SDL_MouseMotionEvent event) noexcept {
            m_previous = m_position;
            m_position.x = event.x;
            m_position.y = event.y;
        }

        [[nodiscard]] inline bool getKeyDown(const uint8_t key) const noexcept {
            return !m_previous_state[key] && m_current_state[key];
        }

        [[nodiscard]] inline bool getKey(const uint8_t key) const noexcept {
            return m_previous_state[key] && m_current_state[key];
        }

        [[nodiscard]] inline bool getKeyUp(const uint8_t key) const noexcept {
            return m_previous_state[key] && !m_current_state[key];
        }

        [[nodiscard]] inline bool isMouseMoved() {
            return m_moved;
        }
    private:
        uint8_t const* const m_keys = SDL_GetKeyboardState(nullptr);
        std::array<uint8_t, 256> m_current_state;
        std::array<uint8_t, 256> m_previous_state;
        glm::ivec2 m_position;
        glm::ivec2 m_previous;
        glm::ivec2 m_delta;
        bool m_moved = false;
        bool m_moved_update = false;
    };
}