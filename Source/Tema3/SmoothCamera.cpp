#include "SmoothCamera.h"

#include "Easing.h"

using StylizedRunner::SmoothCamera;

SmoothCamera::SmoothCamera() {
    SetRotation(Camera3D::GetRotation());
    SetRadius(Camera3D::GetRadius());
    SetFov(Camera3D::GetFov());
}

void SmoothCamera::UpdateActive(float dt, float f, int mods) {
    using namespace Easing;

    if (!IsActive()) {
        return;
    }

    Camera3D::SetRotation(Lerp(Camera3D::GetRotation(), rotation, f * 0.2f));
    Camera3D::SetRadius(Lerp(Camera3D::GetRadius(), radius, f * 0.2f));
    Camera3D::SetFov(Lerp(Camera3D::GetFov(), fov, f * 0.05f));

    Camera3D::SetPosition(Lerp(Camera3D::GetPosition(), position, f * 0.2f));

    Camera3D::UpdateActive(dt, f, mods);
}

glm::vec3 SmoothCamera::GetRotation() const {
    return rotation;
}

void SmoothCamera::SetRotation(glm::vec3 rotation) {
    this->rotation = rotation;
}

float SmoothCamera::GetRadius() const {
    return radius;
}

void SmoothCamera::SetRadius(float radius) {
    this->radius = radius;
}

float SmoothCamera::GetFov() const {
    return fov;
}

void SmoothCamera::SetFov(float fov) {
    this->fov = fov;
}

glm::vec3 SmoothCamera::GetPosition() const {
    return position;
}

void SmoothCamera::SetPosition(glm::vec3 position) {
    this->position = position;
}
