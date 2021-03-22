#pragma once

#include <include/glm.h>

namespace StylizedRunner::Transform3D {
    using namespace glm;

    inline mat4 Translate(vec3 t) {
        mat4 m { 1, 0, 0, t.x,
                 0, 1, 0, t.y,
                 0, 0, 1, t.z,
                 0, 0, 0, 1 };

        return transpose(m);
    }

    inline mat4 RotateX(float radians) {
        float c = cos(radians);
        float s = sin(radians);

        mat4 m { 1, 0,  0, 0,
                 0, c, -s, 0,
                 0, s,  c, 0,
                 0, 0,  0, 1 };

        return transpose(m);
    }

    inline mat4 RotateY(float radians) {
        float c = cos(radians);
        float s = sin(radians);

        mat4 m {  c, 0, s, 0,
                  0, 1, 0, 0,
                 -s, 0, c, 0,
                  0, 0, 0, 1 };

        return transpose(m);
    }

    inline mat4 RotateZ(float radians) {
        float c = cos(radians);
        float s = sin(radians);

        mat4 m { c, -s, 0, 0,
                 s,  c, 0, 0,
                 0,  0, 1, 0,
                 0,  0, 0, 1 };

        return transpose(m);
    }

    inline mat4 Scale(vec3 s) {
        mat4 m { s.x, 0,   0,   0,
                 0,   s.y, 0,   0,
                 0,   0,   s.z, 0,
                 0,   0,   0,   1 };

        return transpose(m);
    }

    inline mat4 Scale(float s) {
        return Scale({ s, s, s });
    }
}
