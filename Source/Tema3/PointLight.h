#pragma once

#include "Light3D.h"

namespace StylizedRunner {
    class PointLight : public Light3D {
    public:
        PointLight() {
            SetDirection({ 0, 0, 0 });
            SetCutoff(0);
        }
    };
}
