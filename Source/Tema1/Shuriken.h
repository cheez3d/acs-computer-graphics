#pragma once

#include "Object2D.h"
#include "Playground.h"

namespace BowAndArrow {
    class Shuriken : public Object2D {
        enum Component : decltype(components)::size_type {
            BLADE_NE,  BLADE_NW,  BLADE_SW,  BLADE_SE,
            HOLE_OUTLINE_NE, HOLE_OUTLINE_NW, HOLE_OUTLINE_SW, HOLE_OUTLINE_SE,
            HOLE_FILLER_NE, HOLE_FILLER_NW, HOLE_FILLER_SW, HOLE_FILLER_SE,
            COMPONENT_COUNT,
        };

    public:
        Shuriken(float bladeSize = 1,
                 float holeSize = 0.5f,
                 float holeOutlineSize = 0.1f);

    protected:
        virtual Shuriken* CloneImplementation() const override;

        virtual void UpdateAlive(float f, int mods) override;

        virtual void UpdatePhysics(float f, int mods) override;

        virtual Playground* GetScene() const override;

    public:
        virtual void CollidedWith(Object2D* other) override;

        virtual void CollidedBalloon(Balloon* balloon) override;
    };
}
