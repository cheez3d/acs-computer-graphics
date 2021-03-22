#pragma once

#include "Camera3D.h"

namespace StylizedRunner {
    class Playground;

    class SmoothCamera : public Camera3D {
        glm::vec3 rotation;
        float radius;
        float fov;

        glm::vec3 position;

    public:
        SmoothCamera();

    protected:
        virtual void UpdateActive(float dt, float f, int mods) override;

    public:
        virtual glm::vec3 GetRotation() const override;
        virtual float GetRadius() const override;
        virtual float GetFov() const override;
        virtual glm::vec3 GetPosition() const override;

        virtual void SetRotation(glm::vec3 rotation) override;
        virtual void SetRadius(float radius) override;
        virtual void SetFov(float fov) override;
        virtual void SetPosition(glm::vec3 position) override;
    };
}
