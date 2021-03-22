#pragma once

#include "Object3D.h"
#include "Playground.h"

namespace Skyroads {
    class Platform final : public Object3D {
        enum Component : decltype(components)::size_type {
            PLANK,
            COMPONENT_COUNT,
        };

    public:
        enum class Type {
            SIMPLE, FUEL_GAIN, FUEL_LOSS, CAMERA_FLIPPED, FORCE_SPEEDUP, FALL,
        };

        static inline glm::vec3 const SIZE { 0.9f, 0.1f, 1.9f };

        static inline int const COLUMN_RADIUS = 3;

        static inline int const ADD_ROW_START = -2;
        static inline int const ADD_ROW_MAX = -10;
        static inline float const ADD_ROW_INCREMENT_DELAY = 15;
        static inline int const FADE_ROW = 2;
        static inline int const REMOVE_ROW = 3;

        static inline int const ADD_EMPTY_ROWS_START = 2;
        static inline int const ADD_EMPTY_ROWS_MAX = 10;
        static inline float const ADD_EMPTY_ROWS_INCREMENT_DELAY = 20;

        static inline glm::vec3 const SIMPLE_COLOR { 0, 97.0f / 255, 181.0f / 255 };
        static inline glm::vec3 const TOUCHED_COLOR { 153.0f / 255, 0, 156.0f / 255 };
        static inline glm::vec3 const FUEL_GAIN_COLOR { 0, 140.0f / 255, 30.0f / 255 };
        static inline glm::vec3 const FUEL_LOSS_COLOR { 1, 111.0f / 255, 0 };
        static inline glm::vec3 const CAMERA_FLIPPED_COLOR { 0, 1, 195.0f / 255 };
        static inline glm::vec3 const FORCE_SPEEDUP_COLOR { 166.0f / 255, 1, 0 };
        static inline glm::vec3 const FALL_COLOR { 224.0f / 255, 0, 56.0f / 255 };

        static inline int const COLLISIONS_ENABLE_ROW = -1;
        static inline int const COLLISIONS_DISABLE_ROW = 0;

        static inline float const FUEL_GAIN = 0.1f;

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

        virtual void UpdateActive(float f, int mods) override;

    public:
        virtual Playground* GetScene() const override;

        virtual glm::vec3 GetColor() const override;
        virtual void SetColor(glm::vec3 color) override;

        virtual float GetAlpha() const override;
        virtual void SetAlpha(float alpha) override;

    protected:
        virtual void UpdatePhysics(float f, int mods) override;

    public:
        Type GetType() const;

        bool IsTouched() const;
        void SetTouched(bool touched);
    };
};
