#pragma once

#include "Object2D.h"
#include "Playground.h"

namespace BowAndArrow {
    class Balloon : public Object2D {
        enum Component : decltype(components)::size_type {
            SHINE, ELLIPSE, TRIANGLE, STRING1, STRING2, STRING3, STRING4,
            COMPONENT_COUNT,
        };

    public:
        enum class Type {
            RED, YELLOW,
        };

    private:
        Type type;

    public:
        Balloon(Type type, float radius = 1);

    protected:
        virtual Balloon* CloneImplementation() const override;

        virtual void UpdateAlive(float f, int mods) override;

        virtual void UpdatePhysics(float f, int mods) override;

    public:
        virtual Playground* GetScene() const override;

    protected:
        virtual void CollidedWith(Object2D* other) override;

        virtual void CollidedBalloon(Balloon* balloon) override;

    public:
        Type GetType() const;
    };
}
