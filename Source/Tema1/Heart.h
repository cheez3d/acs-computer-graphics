#pragma once

#include "Object2D.h"
#include "Playground.h"

#include <chrono>

namespace BowAndArrow {
    class Heart : public Object2D {
        enum Component : decltype(components)::size_type {
            HEART,
            COMPONENT_COUNT,
        };

        float size;
        std::chrono::time_point<std::chrono::system_clock> lastBeat;
        std::chrono::seconds nextBeat;

    public:
        Heart();

    protected:
        virtual Heart* CloneImplementation() const override;

        virtual void UpdateAlive(float f, int mods) override;

    public:
        virtual Playground* GetScene() const override;

    protected:
        virtual void CollidedWith(Object2D* other) override;
    };
}
