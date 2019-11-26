#include "camera.h"

#include "input/input.h"
#include "graphics.h"

namespace {
    module<rosy::input::Input> m_input;
    module<rosy::graphics::Graphics> m_graphics;
}

namespace rosy::graphics {
    glm::vec3 Camera::inverseTransform(glm::vec3 point) {
        return glm::conjugate(transform.rotation.get()) * (
              glm::vec4(point, 1.0f) * transform.localToWorldMatrix()
        );
    }

    void Camera::update(rosy::chrono::duration dt) {
        const float sec = (float) std::chrono::duration<double>(dt).count();

        auto shader = m_graphics->shader;
        if (m_input->isMouseMoved()) {
            shaderState = ShaderState::update_rot;

            const auto mouseDelta = m_input->getMouseDelta();
            const auto delta = -glm::normalize(glm::vec2(mouseDelta)) * sec * 5.0f;

            glm::quat q_up = glm::angleAxis(delta.y, glm::vec3(1,0,0));
            glm::quat q_right = glm::angleAxis(delta.x, glm::vec3(0,1,0));

            transform.rotation = glm::normalize(q_up * (transform.rotation.get() * q_right));
        }

        if (m_input->getKey(SDL_SCANCODE_W)) {
            shaderState ^= ShaderState::move_fb;
            transform.position = transform.position.get() + inverseTransform(transform.forward.m_get()) * 20.0f * sec;
        }
        if (m_input->getKey(SDL_SCANCODE_S)) {
            shaderState ^= ShaderState::move_fb;
            transform.position = transform.position.get() + inverseTransform(transform.backward.m_get()) * 20.0f * sec;
        }
        if (m_input->getKey(SDL_SCANCODE_A)) {
            shaderState ^= ShaderState::move_lr;
            transform.position = transform.position.get() + inverseTransform(transform.left.m_get()) * 20.0f * sec;
        }
        if (m_input->getKey(SDL_SCANCODE_D)) {
            shaderState ^= ShaderState::move_lr;
            transform.position = transform.position.get() + inverseTransform(transform.right.m_get()) * 20.0f * sec;
        }

        if (shaderState & ShaderState::update_pos) {
            shader.fragment.set("camera.position", transform.position.get());
        }

        if (shaderState & ShaderState::update_mat) {
            shader.fragment.set("camera.localToWorldMatrix", transform.localToWorldMatrix());
            shader.fragment.set("camera.cameraToWorldMatrix", cameraToWorldMatrix());
        }

        shaderState = ShaderState::none;
    }
}