#pragma once

#include "Object3D.h"
#include "Playground.h"

#include <chrono>

namespace Skyroads {
    class FuelIndicator final : public Object3D {
        enum Component : decltype(components)::size_type {
            INDICATOR,
            COMPONENT_COUNT,
        };

    public:
        static inline glm::vec3 const SIZE { 1, 0.1f, 0.1f };

        static glm::vec3 const OFFSET;

        static glm::vec3 const FULL_COLOR;
        static glm::vec3 const EMPTY_COLOR;

        static inline float const LEVEL_DECREMENT = 0.02f;
        static inline float const LEVEL_DECREMENT_DELAY = 1;

    private:
        glm::vec3 color;
        float alpha;

        float level;

    public:
        FuelIndicator();

    protected:
        virtual void UpdateActive(float f, int mods) override;

        virtual FuelIndicator* CloneImplementation() const override;

    public:
        virtual bool CollidesWith(Object3D const* other) const override;

    protected:
        virtual void CollidedWith(Object3D* other) override;

    public:
        virtual Playground* GetScene() const override;
        
        virtual glm::vec3 GetColor() const override;
        virtual void SetColor(glm::vec3 color) override;

        virtual float GetAlpha() const override;
        virtual void SetAlpha(float alpha) override;

        float GetLevel() const;
        void SetLevel(float level);
    };
};
