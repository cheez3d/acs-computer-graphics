#pragma once

#include "Light3D.h"

namespace StylizedRunner {
    class SpotLight : public Light3D {
    public:
        SpotLight() {
            SetDirection({ 0, 0, -1 });
            SetCutoff(45);
        }
    };
}
