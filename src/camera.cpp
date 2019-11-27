#include "camera.h"

#include "input/input.h"
#include "rosy/modules/graphics/graphics.h"

namespace {
module<rosy::input::Input> m_input;
module<rosy::graphics::Graphics> m_graphics;
}

glm::vec3 Camera::inverseTransform(glm::vec3 point) {
    return glm::conjugate(transform.rotation.get()) * (
          glm::vec4(point, 1.0f) * transform.localToWorldMatrix()
    );
}

uint8_t Camera::update(rosy::timer::duration dt) {
    const float sec = (float) std::chrono::duration<double>(dt).count();

    uint8_t state = UpdateState::none;

    if (m_input->isMouseMoved()) {
        state = UpdateState::rotate;

        const auto mouseDelta = m_input->getMouseDelta();

        const auto delta = -glm::normalize(glm::vec2(mouseDelta)) * sec * 5.0f;

        glm::quat q_up = glm::angleAxis(delta.y, glm::vec3(1,0,0));
        glm::quat q_right = glm::angleAxis(delta.x, glm::vec3(0,1,0));

        transform.rotation = glm::normalize(q_up * (transform.rotation.get() * q_right));
    }

    if (m_input->getKey(SDL_SCANCODE_W)) {
        state ^= UpdateState::move_fb;
        transform.position = transform.position.get() + inverseTransform(transform.forward.m_get()) * 20.0f * sec;
    }
    if (m_input->getKey(SDL_SCANCODE_S)) {
        state ^= UpdateState::move_fb;
        transform.position = transform.position.get() + inverseTransform(transform.backward.m_get()) * 20.0f * sec;
    }
    if (m_input->getKey(SDL_SCANCODE_A)) {
        state ^= UpdateState::move_lr;
        transform.position = transform.position.get() + inverseTransform(transform.left.m_get()) * 20.0f * sec;
    }
    if (m_input->getKey(SDL_SCANCODE_D)) {
        state ^= UpdateState::move_lr;
        transform.position = transform.position.get() + inverseTransform(transform.right.m_get()) * 20.0f * sec;
    }

    return state;
}