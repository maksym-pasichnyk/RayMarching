#pragma once
//#extension GL_ARB_shading_language_include : require

struct Camera {
    vec3 position;
    mat4 localToWorldMatrix;
    mat4 cameraToWorldMatrix;
};

uniform Camera camera;
uniform ivec2 iResolution;
uniform float iTime;

#define ScreenToWorldPosition(v) \
    vec3(vec4(normalize(vec3(((v - 0.5f  * iResolution) * 1.15470113678f + 1.0f) / iResolution.y, 1)), 1) * camera.cameraToWorldMatrix)

float smin(float a, float b, float k) {
    float h = max(k - abs(a - b), 0);
    return min(a, b) - h * h * 0.25 / k;
}

vec2 smin(vec2 a, vec2 b, float k) {
    float h = clamp(0.5f + 0.5 * (b.x - a.x) / k, 0, 1);
    return mix(b, a, h) - k * h * (1.0f - h);
}

mat4 lookAt(vec3 from, vec3 to, vec3 tmp) {
    vec3 forward = normalize(from - to);
    vec3 right = cross(normalize(tmp), forward);
    vec3 up = cross(forward, right);

    mat4 camToWorld;
    camToWorld[0][0] = right.x;
    camToWorld[0][1] = right.y;
    camToWorld[0][2] = right.z;
    camToWorld[1][0] = up.x;
    camToWorld[1][1] = up.y;
    camToWorld[1][2] = up.z;
    camToWorld[2][0] = forward.x;
    camToWorld[2][1] = forward.y;
    camToWorld[2][2] = forward.z;
    camToWorld[3][0] = from.x;
    camToWorld[3][1] = from.y;
    camToWorld[3][2] = from.z;
    return camToWorld;
}

mat3 rotate(float y, float p, float r) {
    mat3 A = mat3(cos(r), sin(r), 0, -sin(r), cos(r), 0, 0, 0, 1);
    mat3 B = mat3(cos(p), 0, sin(p), 0, 1, 0, -sin(p), 0, cos(p));
    mat3 C = mat3(1, 0, 0, 0, cos(y), sin(y), 0, -sin(y), cos(y));
    return A * B * C;
}

vec3 rotatePoint(vec3 point, vec3 center, mat3 rot) {
    return rot * (point -  center) + center;
}