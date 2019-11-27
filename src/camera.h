#pragma once

#include <glm/vec3.hpp>
#include <rosy/property.h>
#include <glm/mat4x4.hpp>
#include <src/components/transform.h>
#include "timer/timer.h"

struct Camera {
public:
    enum UpdateState : uint8_t {
        none      = 0u,
        rotate    = 1u,
        move_fb   = 2u,
        move_lr   = 4u,
        updatePos = 6u,
        updateMat = 7u,
    };

    [[nodiscard]] inline glm::mat4 cameraToWorldMatrix() const noexcept {
        return transform.inverseWorldToLocalMatrix();
    }

    uint8_t update(rosy::timer::duration dt);
    glm::vec3 inverseTransform(glm::vec3 point);
    Transform transform{};

private:
    uint8_t updateState = UpdateState::none;
};