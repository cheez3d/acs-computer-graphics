#pragma once

#include "SmoothCamera.h"
#include "Timer.h"

namespace StylizedRunner {
    class Player;
    class Playground;

    class PlayerCamera : public SmoothCamera {
    public:
        static constexpr glm::vec3 START_ROTATION { glm::radians<float>(30), glm::pi<float>(), 0 };

        static constexpr float RADIUS_MIN = 0;
        static constexpr float RADIUS_MAX = 5;

        static constexpr glm::vec3 FLIP_ROTATION_BASE { glm::radians<float>(10), glm::pi<float>(), 0 };
        static constexpr float FLIP_ROTATION = glm::half_pi<float>();

    private:
        bool isFlipped;
        Timer<float> flipTimer;

    public:
        PlayerCamera();

    protected:
        virtual void UpdateActive(float dt, float f, int mods) override;

    private:
        using Camera3D::GetObject;
        using Camera3D::GetScene;

        using Camera3D::SetObject;
        virtual void SetScene(Scene3D* scene) override;

    public:
        Player* GetObject() const;
        Playground* GetScene() const;

        void SetObject(Player* object);
        void SetScene(Playground* scene);

        virtual void SetRotation(glm::vec3 rotation) override;
        virtual void SetRadius(float radius) override;

        bool IsFlipped() const;

        void SetFlipped(bool flipped);

        void ResetTimes();
    };
}
