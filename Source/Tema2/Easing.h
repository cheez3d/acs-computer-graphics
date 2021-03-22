#pragma once

#include <include/glm.h>

namespace Easing {
    using namespace glm;

    inline float Lerp(float begin, float end, float t) {
        t = clamp<float>(t, 0, 1);

        return (1 - t) * begin + t * end;
    }

    inline vec2 Lerp(vec2 begin, vec2 end, float t) {
        t = clamp<float>(t, 0, 1);

        return (1 - t) * begin + t * end;
    }

    inline vec3 Lerp(vec3 begin, vec3 end, float t) {
        t = clamp<float>(t, 0, 1);

        return (1 - t) * begin + t * end;
    }
}
