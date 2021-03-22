#pragma once

#include "PlaygroundObject.h"

namespace StylizedRunner {
    class Playground;

    class Platform final : public PlaygroundObject {
        enum Component : decltype(components)::size_type {
            PLANK,
            COMPONENT_COUNT,
        };

    public:
        enum class Type {
            CAMERA_FLIP, FALL, FORCE_SPEEDUP, FUEL_GAIN, FUEL_LOSS, SIMPLE,
        };

        static constexpr glm::vec3 SIZE { 1, 1, 1 };

        static constexpr int COLUMN_RADIUS = 5;

        static constexpr int ADD_ROW_START = -4;
        static constexpr int ADD_ROW_MIN = -20;
        static constexpr int COLLISIONS_ENABLE_ROW = -1;
        static constexpr int COLLISIONS_DISABLE_ROW = 6;
        static constexpr int FADE_ROW = COLLISIONS_DISABLE_ROW;
        static constexpr int REMOVE_ROW = FADE_ROW + 1;

        static constexpr int ADD_EMPTY_ROWS_START = 2;
        static constexpr int ADD_EMPTY_ROWS_MAX = 10;

        static constexpr glm::vec3 SIMPLE_COLOR { 0, 97.0f / 255, 181.0f / 255 };
        static constexpr glm::vec3 TOUCHED_COLOR { 153.0f / 255, 0, 156.0f / 255 };
        static constexpr glm::vec3 FUEL_GAIN_COLOR { 0, 140.0f / 255, 30.0f / 255 };
        static constexpr glm::vec3 FUEL_LOSS_COLOR { 1, 111.0f / 255, 0 };
        static constexpr glm::vec3 CAMERA_FLIPPED_COLOR { 0, 1, 195.0f / 255 };
        static constexpr glm::vec3 FORCE_SPEEDUP_COLOR { 166.0f / 255, 1, 0 };
        static constexpr glm::vec3 FALL_COLOR { 224.0f / 255, 0, 56.0f / 255 };

        static constexpr float FUEL_GAIN = 0.1f;

        static Type GetRandomType();

    private:
        glm::vec3 color;
        float alpha;

        Type type;
        bool isTouched;

    public:
        Platform(Type type);
        Platform();

    protected:
        virtual Platform* CloneImplementation() const override;

    public:
        virtual Bounding::Box* GetBounding() const override;

        virtual bool CollidesWith(Object3D const* other) const override;

    protected:
        virtual void CollidedWith(Object3D* other) override;

        virtual void CollidedPlayer(Player* player) override;

        virtual void UpdateActive(float dt, float f, int mods) override;

    public:
        Playground* GetScene() const;

        virtual glm::vec3 GetColor() const override;
        virtual void SetColor(glm::vec3 color) override;

        virtual float GetAlpha() const override;
        virtual void SetAlpha(float alpha) override;

    protected:
        virtual void UpdatePhysics(float dt, float f, int mods) override;

    public:
        Type GetType() const;

        bool IsTouched() const;
        void SetTouched(bool touched);
    };
};
