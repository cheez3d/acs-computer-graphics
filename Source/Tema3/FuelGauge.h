#pragma once

#include "Platform.h"
#include "PlaygroundObject.h"

#include <memory>

namespace StylizedRunner {
    class Playground;

    class FuelGauge final : public PlaygroundObject {
        friend class Player;

        enum Component : decltype(components)::size_type {
            INDICATOR,
            COMPONENT_COUNT,
        };

    public:
        static constexpr glm::vec3 SIZE { 1.95f, 0.1f, 0 };

        static constexpr glm::vec3 FuelGauge::FULL_COLOR { Platform::FUEL_GAIN_COLOR };
        static constexpr glm::vec3 FuelGauge::EMPTY_COLOR { Platform::FUEL_LOSS_COLOR };

        static constexpr float LEVEL_DECREMENT = 0.01f;

    private:
        glm::vec3 color;
        float alpha;

        float level;

    public:
        FuelGauge();

    protected:
        virtual void UpdateActive(float dt, float f, int mods) override;

    private:
        virtual FuelGauge* CloneImplementation() const override;

        using Object3D::Clone;

    public:
        std::unique_ptr<FuelGauge> Clone() const;

        virtual bool CollidesWith(Object3D const* other) const override;

    protected:
        virtual void CollidedWith(Object3D* other) override;

    public:
        Playground* GetScene() const;
        
        virtual glm::vec3 GetColor() const override;
        virtual void SetColor(glm::vec3 color) override;

        virtual float GetAlpha() const override;
        virtual void SetAlpha(float alpha) override;

        float GetLevel() const;
        void SetLevel(float level);
    };
};
