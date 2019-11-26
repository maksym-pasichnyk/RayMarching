#pragma once

#include "transform.h"

#include "timer/timer.h"

namespace rosy::graphics {
    struct Camera {
    public:
        [[nodiscard]] inline glm::mat4 projectionMatrix() const noexcept {
            if (m_dirty) {
                m_dirty = false;
                m_projection_matrix = glm::perspective(m_fov, m_aspect, m_near, m_far);
            }
            return m_projection_matrix;
        }

        [[nodiscard]] inline glm::mat4 cameraToWorldMatrix() const noexcept {
            return transform.inverseWorldToLocalMatrix();
        }

        inline void setSize(const int width, const int height) noexcept {
            aspect = float(width) / float(height);
        }

        property<float, property<>::set> aspect = [&](const float v) {
            if (m_aspect != v) {
                m_dirty = true;
                m_aspect = v;
            }
        };

        property<float, property<>::set> near = [&](const float v) {
            if (m_near != v) {
                m_dirty = true;
                m_near = v;
            }
        };

        property<float, property<>::set> far = [&](const float v) {
            if (m_far != v) {
                m_dirty = true;
                m_far = v;
            }
        };

        property<float, property<>::set> fov = [&](const float v) {
            if (m_fov != v) {
                m_dirty = true;
                m_fov = v;
            }
        };

        void update(rosy::chrono::duration dt);
        glm::vec3 inverseTransform(glm::vec3 point);
        Transform transform{};

    private:
        enum ShaderState : uint8_t {
            none       = 0u,
            update_rot = 1u,
            move_fb    = 2u,
            move_lr    = 4u,
            update_pos = 6u,
            update_mat = 7u,
            update_all = 7u,
        };

        uint8_t shaderState = ShaderState::none;

        mutable glm::mat4 m_projection_matrix;
        mutable bool m_dirty = true;

        float m_aspect = 4.0f / 3.0f;
        float m_near = 0.01f;
        float m_far = 1000.0f;
        float m_fov = glm::radians(45.0f);
    };
}