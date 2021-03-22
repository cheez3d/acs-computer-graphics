#pragma once

#include "Object3D.h"
#include "Playground.h"

#include <chrono>

namespace Skyroads {
    class Player final : public Object3D {
        enum Component : decltype(components)::size_type {
            SPHERE,
            COMPONENT_COUNT,
        };

    public:
        static inline float const DIAMETER = 0.5f;

        static inline int const COLUMN_START = 0;
        static glm::vec3 const POSITION_START;

        static inline glm::vec3 const COLOR { 1, 74.0f / 255, 249.0f / 255 };

        static inline int const FALL_FIRST_PERSON_Y = -1;
        static inline int const FALL_RESET_Y = -32;

        static inline float const JUMP_TIME_MAX = 0.15f;

        static inline float const SPEED_START = 0.02f;
        static inline float const SPEED_MAX = 0.2f;
        static inline float const SPEED_INCREMENT = 0.0025f;
        static inline float const SPEED_INCREMENT_DELAY = 5;
        static inline float const SPEEDUP = 2;
        static inline float const FORCE_SPEEDUP = 3;
        static inline float const FORCE_SPEEDUP_DURATION = 5;

    private:
        glm::vec3 color;
        float alpha;

        int column;
        std::chrono::time_point<std::chrono::steady_clock> lastJump;

        bool isSpecial;

    public:
        Player();

    protected:
        virtual Player* CloneImplementation() const override;

    public:
        virtual Bounding::Sphere* GetBounding() const override;

        virtual bool CollidesWith(Object3D const* other) const override;

    protected:
        virtual void CollidedWith(Object3D* other) override;

        virtual void CollidedPlatform(Platform* platform) override;

        virtual void KeyChange(int key, int mods, bool press) override;

        virtual void UpdateActive(float f, int mods) override;

    public:
        virtual glm::vec3 GetColor() const override;
        virtual void SetColor(glm::vec3 color) override;

        virtual float GetAlpha() const override;
        virtual void SetAlpha(float alpha) override;

        virtual Playground* GetScene() const override;

    protected:
        virtual void UpdatePhysics(float f, int mods);

        virtual void PrepareShader(Shader* shader, glm::mat4 model, int component) const override;

    public:
        int GetColumn() const;

        bool IsSpecial() const;
        void SetSpecial(bool special);

        void KillCamera();
        void Kill();
        void Reset();
    };
}
