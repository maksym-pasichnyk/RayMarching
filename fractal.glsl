#pragma once

#define MAX_STEPS 1000
#define MAX_DIST 1000.0f
#define SURFACE_DIST 1e-3f

uniform float fractal_power = 10;
uniform float darkness = 40;
uniform vec3 fractal_color = vec3(1, 0, 1);

struct appdata {
    vec2 position;
    vec2 uv;
};

struct v2f {
    vec2 uv;
};

in appdata in_data;
out v2f o;
in v2f i;

#ifdef VERTEX_SHADER
    void main() {
        o.uv = in_data.uv;
        gl_Position = vec4(in_data.position, 0.0, 1.0);
    }
#else
    vec4 render(in vec2 uv);
    void main() {
        gl_FragColor = render(i.uv);
    }
#endif

vec2 scene(vec3 p) {
    vec3 z = p;
    float dr = 1.0f;
    float r = 0.0f;
    int iterations = 0;

    while (iterations < 15) {
        r = length(z);
        if (r > 2) break;


        float theta = acos(z.z / r) - _Time / 5;
        float phi = atan(z.y, z.x);// * cos(_Time);
        dr =  pow(r, fractal_power - 1.0f) * fractal_power * dr + 1.0f;

        float zr = pow(r, fractal_power);
        theta = theta * fractal_power;
        phi = phi * fractal_power;

        z = zr * vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta)) + p;

        iterations++;
    }
    return vec2(iterations, log(r) * r / dr * 0.5f);
}

vec3 getNormal(vec3 p) {
    vec2 eps = vec2(SURFACE_DIST, 0);

    return normalize(vec3(
        scene(p + eps.xyy).y - scene(p - eps.xyy).y,
        scene(p + eps.yxx).y - scene(p - eps.yxx).y,
        scene(p + eps.yyx).y - scene(p - eps.yyx).y
    ));
}

vec4 render(in vec2 uv) {
    mat3 rot = rotate(0, _Time / 10.0f, 0);
    vec3 pos = rot * _WorldSpaceCameraPos;
    vec3 dir = rot * ScreenToWorldPosition(uv);

    vec3 col = mix(vec3(51, 3, 20), vec3(16, 6, 28), uv.y) / 255.0;

    float dst = 0;

    int i = 0;
    while (dst < MAX_DIST && i < MAX_STEPS) {
        vec2 scene = scene(pos);
        float d = scene.y;

        if (d <= SURFACE_DIST) {
            col = fractal_color * clamp(scene.x / 16.0, 0, 1);
            break;
        }

        pos += dir * d;
        dst += d;
        i++;
    }

    return vec4(col, 1) * i / darkness;
}