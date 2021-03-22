#pragma once

#include "Object2D.h"
#include "Playground.h"

namespace BowAndArrow {
    class Arrow : public Object2D
    {
        enum Component : decltype(components)::size_type {
            TIP, BACK, POWER_BAR, STICK,
            COMPONENT_COUNT,
        };

        float power;

    public:
        Arrow();

    protected:
        virtual Arrow* CloneImplementation() const override;

        virtual void UpdatePhysics(float f, int mods) override;

    public:
        virtual Playground* GetScene() const override;

    protected:
        virtual void CollidedWith(Object2D* other) override;

        virtual void CollidedBalloon(Balloon* balloon) override;
        virtual void CollidedShuriken(Shuriken* shuriken) override;

    public:
        void SetPower(float power);
    };
}
