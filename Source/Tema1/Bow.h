#pragma once

#include "Arrow.h"
#include "Object2D.h"
#include "Playground.h"

#include <chrono>

namespace BowAndArrow {
    class Bow : public Object2D {
        enum Component : decltype(components)::size_type {
            ARC, UPPER_STRING, LOWER_STRING,
            COMPONENT_COUNT,
        };

        enum Attachment : decltype(attachments)::size_type {
            ARROW,
        };

    public:
        static inline float const SHOT_DELAY = 1;

    private:
        bool isLoaded;
        float power;
        float elevation;
        std::chrono::time_point<std::chrono::steady_clock> lastShot;

    public:
        Bow();

    protected:
        virtual Bow* CloneImplementation() const override;

        virtual void UpdateAlive(float f, int mods) override;

        virtual void MouseChange(int btn, int mods, bool press) override;

    public:
        virtual Playground* GetScene() const override;

    protected:
        virtual void CollidedWith(Object2D* other) override;

        virtual void CollidedShuriken(Shuriken* shuriken) override;

    public:
        bool IsLoaded() const;
        std::unique_ptr<Arrow> SetLoaded(bool loaded);

        float GetPower() const;
        void SetPower(float power);
    };
}
