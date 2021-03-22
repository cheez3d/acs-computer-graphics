#include "PlayerCamera.h"

#include "Player.h"
#include "Playground.h"
#include "Random.h"

using StylizedRunner::Player;
using StylizedRunner::PlayerCamera;
using StylizedRunner::Playground;

PlayerCamera::PlayerCamera() {
    Camera3D::SetRotation(START_ROTATION); SetRotation(Camera3D::GetRotation());

    SetFlipped(false);

    flipTimer.SetDuration(7);

    ResetTimes();
}

void PlayerCamera::SetRotation(glm::vec3 rotation) {
    using namespace glm;

    rotation.x = clamp<float>(
        rotation.x,
        abs(0 - GetRadius()) <= epsilon<float>() ? radians<float>(-60) : 0,
        radians<float>(85)
    );

    //rotation.y = clamp(rotation.y, radians<float>(110), radians<float>(250));

    //if (abs(rotation.y) > pi<float>()) {
    //    rotation.y = atan(sin(rotation.y), cos(rotation.y));
    //}

    //if (abs(rotation.z) > pi<float>()) {
    //    rotation.z = atan(sin(rotation.z), cos(rotation.z));
    //}

    SmoothCamera::SetRotation(rotation);
}

void PlayerCamera::SetRadius(float radius) {
    using namespace glm;

    radius = clamp<float>(radius, RADIUS_MIN, RADIUS_MAX);

    SmoothCamera::SetRadius(radius);

    SetRotation(GetRotation()); // for radius checks

    if (GetObject()) {
        GetObject()->SetAlpha(radius <= epsilon<float>() ? 0.0f : 1.0f);
    }
}

void PlayerCamera::UpdateActive(float dt, float f, int mods) {
    SmoothCamera::UpdateActive(dt, f, mods);

    if (!IsActive()) {
        return;
    }

    if (isFlipped && flipTimer.IsElapsed()) {
        SetFlipped(false);

        flipTimer.SetElapsed(false);
    }
}

Player* PlayerCamera::GetObject() const {
    return static_cast<Player*>(Camera3D::GetObject());
}

void PlayerCamera::SetObject(Player* object) {
    Camera3D::SetObject(object);
}

Playground* PlayerCamera::GetScene() const {
    return static_cast<Playground*>(Camera3D::GetScene());
}

void PlayerCamera::SetScene(Scene3D* scene) {
    Camera3D::SetScene(scene);

    auto watch = scene ? scene->GetWatch() : nullptr;

    flipTimer.SetWatch(watch);
}

void PlayerCamera::SetScene(Playground* scene) {
    SetScene(static_cast<Scene3D*>(scene));
}

bool PlayerCamera::IsFlipped() const {
    return isFlipped;
}

void PlayerCamera::SetFlipped(bool flipped) {
    static glm::vec3 savedRotation = GetRotation();
    static float savedRadius = GetRadius();

    SetControllable(!flipped);

    if (flipped) {
        if (!this->isFlipped) {
            savedRotation = GetRotation();
            savedRadius = GetRadius();
        }

        SetRotation(FLIP_ROTATION_BASE);
        SetRotationZ((Random::Int() % 2 ? -1 : Random::Int(1, 2)) * FLIP_ROTATION);

        SetRadius(RADIUS_MAX);

        flipTimer.SetElapsed(false);
    }
    else {
        SetRotation(savedRotation);
        SetRadius(savedRadius);
    }

    this->isFlipped = flipped;
}

void PlayerCamera::ResetTimes() {
    flipTimer.SetElapsed(false);
}
