#pragma once

#define MAX_STEPS 100
#define MAX_DIST 1000.0f
#define SURFACE_DIST 1e-3f

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
    vec4 getColor(vec2 i);
    void main() {
        gl_FragColor = getColor(i.uv);
    }
#endif

struct Ray {
    vec3 orig;
    vec3 dir;
};

struct Light {
    vec3 position;
    float intensity;
};

struct Material {
    vec4 albedo;
    vec3 diffuse_color;
    float specular_exponent;
    float refractive_index;
};

struct Sphere {
    vec3 center;
    float radius;
};

struct Box {
    vec3 center;
    vec3 size;
};

struct Plane {
    vec3 normal;
    vec3 center;
};

struct Capsule {
    vec3 start;
    vec3 up;
    vec2 size;
};

struct Cylinder {
    vec3 start;
    vec3 up;
    vec2 size;
};

struct Torus {
    vec3 center;
    vec2 radius;
};

struct RaycastHit {
    vec3 point;
    vec3 normal;
    float dist;
};

Sphere[] spheres = Sphere[](
    Sphere(vec3(0, 2, 0), 2.0f)
);

Capsule[] capsules = Capsule[] (
    Capsule(vec3(0, 0, 0), vec3(0, 1, 0), vec2(0.5f, 3))
);

Torus[] toruses = Torus[] (
    Torus(vec3(0, 2, 0), vec2(0.2f, 2.0f))
);

Box[] boxes = Box[] (
    Box(vec3(0, 2, 0), vec3(3, 1, 1))
);

Cylinder[] cylinders = Cylinder[] (
    Cylinder(vec3(0, 2, -3), normalize(vec3(0, 0, 1)), vec2(0.2f, 3))
);

Plane[] planes = Plane[](
    Plane(vec3(0, 1, 0), vec3(0, -5, 0))
);

Light[] lights = Light[](
    Light(vec3(-20, 5, 0), 1.0f)
);

vec3 offset(vec3 dir, vec3 normal, vec3 point, float offset) {
    if (dot(dir, normal) < 0.0f) {
        return point - normal * offset;
    }
    return point + normal * offset;
}

float sphereSDF(vec3 point, Sphere obj) {
    float r = obj.radius;
    vec3 C = obj.center;

    return length(point - C) - r;
}

float capsuleSDF(vec3 point, Capsule obj) {
    vec3 ab = obj.up * obj.size.y;
    vec3 start = obj.start + obj.up;

    vec3 ap = point - start;

    float r = obj.size.x;
    float t = clamp(dot(ab, ap) / dot(ab, ab), 0, 1);
    vec3 C = start + t * ab;

    return length(point - C) - r;
}

float cylinderSDF(vec3 point, Cylinder obj) {
    vec3 dir = obj.up * obj.size.y;
    vec3 start = obj.start + obj.up;

    vec3 ap = (point - start);

    float h = obj.size.y;
    float h2 = h * h;

    float r = obj.size.x;
    float t = dot(dir, ap) / h2;
    vec3 C = start + t * dir;
    float x = length(point - C) - r;
    float y = (abs(t - 0.5f) - 0.5f) * length(dir);
    float e = length(max(vec2(x, y), 0));
    return e + min(max(x, y), 0);
}

float torusSDF(vec3 point, Torus obj) {
    float R = obj.radius.y;
    float r = obj.radius.x;
    vec3 C = point - obj.center;
    float P = length(C.xz) - R;
    return length(vec2(P, C.y)) - r;
}

float boxSDF(vec3 point, Box obj) {
    vec3 C = abs(obj.center - point) - obj.size;
    float inside = min(max(C.x, max(C.y, C.z)), 0);
    float outside = length(max(C, 0));
    return inside + outside;
}

float scene(vec3 point) {
    float d = sphereSDF(point, spheres[0]);
    for (int i = 1; i < spheres.length(); i++) {
        d = smin(d, sphereSDF(point, spheres[i]), 0.5f);
    }
    for (int i = 0; i < capsules.length(); i++) {
        d = smin(d, capsuleSDF(point, capsules[i]), 0.5f);
    }
    for (int i = 0; i < toruses.length(); i++) {
        d = smin(d, torusSDF(point, toruses[i]), 0.5f);
    }
    for (int i = 0; i < boxes.length(); i++) {
        d = max(d, -boxSDF(point, boxes[i]));
    }
    for (int i = 0; i < cylinders.length(); i++) {
        d = smin(d, cylinderSDF(point, cylinders[i]), 0.5f);
    }

    return min(d, point.y);
}

vec3 getNormal(vec3 point) {
    vec2 e = vec2(SURFACE_DIST, 0);

    return normalize(
        scene(point) - vec3(
            scene(point - e.xyy),
            scene(point - e.yxy),
            scene(point - e.yyx)
        )
    );
}

float render(in Ray ray) {
    vec3 p = ray.orig;
    float dist = 0;
    for (int i = 0; dist < MAX_DIST && i < MAX_STEPS; i++) {
        float d = scene(p);
        dist += d;
        p += ray.dir * d;
        if (dist < SURFACE_DIST) break;
    }
    return dist;
}

vec2 getLight(vec3 point) {
    float diffuse = 0.0f;
    float specular = 0.0f;

    for (int i = 0; i < lights.length(); i++) {
        Light light = lights[i];
        vec3 light_dir = light.position - point;
        vec3 light_norm = normalize(light_dir);
        float light_dist = dot(light_dir, light_dir);

        vec3 normal = getNormal(point);

        float L = dot(light_norm, normal);
        if (L > 0.0f) {
            vec3 orig = offset(light_norm, normal, point, SURFACE_DIST * 2.0f);
            Ray ray = Ray(orig, light_norm);
            if (render(Ray(orig, light_norm)) >= length(light_dir)) {
                diffuse += L * light.intensity;
                //                specular += pow(max(0.f, dot(reflect(light_norm, normal), ray_dir)), specular_exponent) * light.intensity;
            }
        }
    }

    return vec2(diffuse, specular);
}

vec4 getColor(in vec2 i) {
    vec3 ray_orig = _WorldSpaceCameraPos;
    vec3 ray_dir = ScreenToWorldPosition(i);

    float dist = render(Ray(ray_orig, ray_dir));

    vec3 point = ray_orig + ray_dir * dist;

    vec2 light = getLight(point);
    vec3 col = vec3(light.x);

    return vec4(col, 1);
}