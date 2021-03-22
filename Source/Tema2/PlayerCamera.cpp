#include "PlayerCamera.h"

#include "Easing.h"
#include "Playground.h"
#include "Random.h"

using Skyroads::Player;
using Skyroads::PlayerCamera;
using Skyroads::Playground;

PlayerCamera::PlayerCamera() {
    using namespace glm;

    Camera3D::SetRotation(START_ROTATION);

    SetRotation(Camera3D::GetRotation());
    SetRadius(Camera3D::GetRadius());
    SetFov(Camera3D::GetFov());

    SetFlipped(false);

    ResetTimes();
}

Player* PlayerCamera::GetTarget() const {
    return static_cast<Player*>(Camera3D::GetTarget());
}

glm::vec3 PlayerCamera::GetRotation() const {
    return rotation;
}

void PlayerCamera::SetRotation(glm::vec3 rotation) {
    using namespace glm;

    rotation.x = clamp<float>(
        rotation.x,
        radius <= epsilon<float>() ? radians<float>(-60) : 0,
        radians<float>(85)
     );

    rotation.y = clamp(rotation.y, radians<float>(110), radians<float>(250));

    //if (abs(rotation.y) > pi<float>()) {
    //    rotation.y = atan(sin(rotation.y), cos(rotation.y));
    //}

    //if (abs(rotation.z) > pi<float>()) {
    //    rotation.z = atan(sin(rotation.z), cos(rotation.z));
    //}

    this->rotation = rotation;
}

float PlayerCamera::GetRadius() const {
    return radius;
}

void PlayerCamera::SetRadius(float radius) {
    using namespace glm;

    radius = clamp<float>(radius, RADIUS_MIN, RADIUS_MAX);

    this->radius = radius;

    SetRotation(GetRotation()); // for radius checks

    if (GetTarget()) {
        GetTarget()->SetAlpha(radius <= epsilon<float>() ? 0.0f : 1.0f);
    }
}

float PlayerCamera::GetFov() const {
    return fov;
}

void PlayerCamera::SetFov(float fov) {
    this->fov = fov;
}

glm::vec3 PlayerCamera::GetPosition() const {
    return position;
}

void PlayerCamera::SetPosition(glm::vec3 position) {
    this->position = position;
}

Playground* PlayerCamera::GetScene() const {
    return static_cast<Playground*>(Camera3D::GetScene());
}

void PlayerCamera::UpdateActive(float f, int mods) {
    using namespace std::chrono;
    using namespace Easing;

    if (IsActive()) {
        Camera3D::SetRotation(Lerp(Camera3D::GetRotation(), rotation, f * 0.2f));
        Camera3D::SetRadius(Lerp(Camera3D::GetRadius(), radius, f * 0.2f));
        Camera3D::SetFov(Lerp(Camera3D::GetFov(), fov, f * 0.05f));

        Camera3D::SetPosition(Lerp(Camera3D::GetPosition(), position, f * 0.2f));

        auto t = GetScene()->GetTime();

        if (isFlipped &&
            duration_cast<duration<float>>(t - lastFlipped).count() >= FLIPPED_DURATION)
        {
            SetFlipped(false);
        }
    }
}

bool PlayerCamera::IsFlipped() const {
    return isFlipped;
}

void PlayerCamera::SetFlipped(bool flipped) {
    static glm::vec3 savedRotation = START_ROTATION;
    static float savedRadius = Camera3D::DEFAULT_RADIUS;

    SetControllable(!flipped);

    if (flipped) {
        if (!this->isFlipped) {
            savedRotation = rotation;
            savedRadius = radius;
        }

        SetRotation(FLIPPED_ROTATION_BASE);
        SetRotationZ((Random::Int() % 2 ? -1 : Random::Int(1, 2)) * FLIPPED_ROTATION);

        SetRadius(RADIUS_MAX);

        lastFlipped = GetScene()->GetTime();
    }
    else {
        SetRotation(savedRotation);
        SetRadius(savedRadius);
    }

    this->isFlipped = flipped;
}

void PlayerCamera::ResetTimes() {
    using namespace std::chrono;

    lastFlipped = steady_clock::now();
}
