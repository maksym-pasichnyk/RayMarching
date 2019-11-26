#pragma once

#include "property.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <timer/timer.h>
#include <iostream>

#include <list>

static glm::mat4 TRS(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
    return glm::scale(glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation), scale);
}

struct Transform {
public:
    property<glm::vec3, property<>::get, property<>::set> position = {
        [&] { return m_worldPosition; },
        [&](const glm::vec3 &v) {
            if (m_worldPosition != v) {
                m_dirty = true;
                m_worldPosition = v;
            }
        }
    };

    property<glm::quat, property<>::get, property<>::set> rotation = {
            [&] { return m_worldRotation; },
            [&] (const glm::quat &v) {
                if (m_worldRotation != v) {
                    m_dirty = true;
                    m_worldRotation = v;
                }
            }
    };

    property<glm::vec3, property<>::get> forward = [&] {
        if (m_dirty) updateMatrices();
        return m_forward;
    };

    property<glm::vec3, property<>::get> backward = [&] {
        if (m_dirty) updateMatrices();
        return m_backward;
    };

    property<glm::vec3, property<>::get> up = [&] {
        if (m_dirty) updateMatrices();
        return m_up;
    };

    property<glm::vec3, property<>::get> down = [&] {
        if (m_dirty) updateMatrices();
        return m_down;
    };

    property<glm::vec3, property<>::get> left = [&] {
        if (m_dirty) updateMatrices();
        return m_left;
    };

    property<glm::vec3, property<>::get> right = [&] {
        if (m_dirty) updateMatrices();
        return m_right;
    };

    glm::mat4 localToWorldMatrix() const {
        if (m_dirty) updateMatrices();
        return m_localToWorldMatrix;
    }

    glm::mat4 worldToLocalMatrix() const {
        if (m_dirty) updateMatrices();
        return m_worldToLocalMatrix;
    };

    glm::mat4 inverseWorldToLocalMatrix() const {
        if (m_dirty) updateMatrices();
        return m_inverseWorldToLocalMatrix;
    };

private:
    mutable glm::vec3 m_worldPosition = {0, 0, 0};
    mutable glm::quat m_worldRotation = {1, 0, 0, 0};
//    mutable glm::vec3 m_worldScale = {1, 1, 1};

    mutable glm::vec3 m_forward, m_backward;
    mutable glm::vec3 m_up, m_down;
    mutable glm::vec3 m_left, m_right;

    mutable glm::mat4 m_localToWorldMatrix;
    mutable glm::mat4 m_worldToLocalMatrix;
    mutable glm::mat4 m_inverseWorldToLocalMatrix;

    mutable bool m_dirty = true;

    void updateMatrices() const {
        m_dirty = false;

        m_localToWorldMatrix = TRS(position.get(), rotation.get(), {1, 1, 1});
        m_worldToLocalMatrix = TRS(position.get(), glm::conjugate(rotation.get()), {1, 1, -1});
        m_inverseWorldToLocalMatrix = glm::inverse(m_worldToLocalMatrix);

        m_backward = -(m_forward = (rotation.get() * glm::vec3{0, 0, -1}));
        m_down = -(m_up = rotation.get() * glm::vec3{0, 1, 0});
        m_left = -(m_right = rotation.get() * glm::vec3{1, 0, 0});
    }
};
