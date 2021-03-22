#pragma once

#include "Camera3D.h"
#include "Player.h"
#include "Playground.h"

#include <chrono>

namespace Skyroads {
    class Playground;

    class PlayerCamera final : public Camera3D {
    public:
        static inline glm::vec3 const START_ROTATION { glm::radians<float>(30), glm::pi<float>(), 0 };

        static inline float const RADIUS_MIN = 0;
        static inline float const RADIUS_MAX = 3;

        static inline glm::vec3 const FLIPPED_ROTATION_BASE { glm::radians<float>(10), glm::pi<float>(), 0 };
        static inline float const FLIPPED_ROTATION = glm::half_pi<float>();
        static inline float const FLIPPED_DURATION = 10;

    private:
        glm::vec3 rotation;
        float radius;
        float fov;

        glm::vec3 position;

        bool isFlipped;
        std::chrono::time_point<std::chrono::steady_clock> lastFlipped;

    public:
        PlayerCamera();

        virtual Player* GetTarget() const override;

        virtual glm::vec3 GetRotation() const override;
        virtual void SetRotation(glm::vec3 rotation) override;

        virtual float GetRadius() const override;
        virtual void SetRadius(float radius) override;

        virtual float GetFov() const override;
        virtual void SetFov(float fov) override;

        virtual glm::vec3 GetPosition() const override;
        virtual void SetPosition(glm::vec3 position) override;

    public:
        virtual Playground* GetScene() const override;

    protected:
        virtual void UpdateActive(float f, int mods) override;

    public:
        bool IsFlipped() const;
        void SetFlipped(bool flipped);

        void ResetTimes();
    };
}
