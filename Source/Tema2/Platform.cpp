#include "Platform.h"

#include "Easing.h"
#include "FuelIndicator.h"
#include "Object3D.h"
#include "Player.h"
#include "PlayerCamera.h"
#include "Random.h"
#include "Transform3D.h"

using Skyroads::Collidable;
using Skyroads::Object3D;
using Skyroads::Platform;
using Skyroads::Playground;

Platform::Type Platform::GetRandomType() {
    float p = 100 * Random::Float();

    return p <= 70    ? Type::SIMPLE         :
           p <= 85    ? Type::FUEL_GAIN      :
           p <= 90    ? Type::CAMERA_FLIPPED :
           p <= 95    ? Type::FORCE_SPEEDUP  :
           p <= 97.5f ? Type::FUEL_LOSS      :
                        Type::FALL           ;
}

Platform::Platform(Type type) {
    using namespace Transform3D;

    components.resize(COMPONENT_COUNT);

    components[PLANK] = { "Box", Scale(SIZE), mat4 { 1 } };

    auto bounding = std::make_unique<Bounding::Box>();
    bounding->SetPosition(GetBounding()->GetPosition());
    bounding->SetOffset(GetBounding()->GetOffset());
    bounding->SetSize(SIZE);

    SetBounding(std::move(bounding));

    Object3D::SetColor(
        type == Type::FUEL_GAIN      ? FUEL_GAIN_COLOR      :
        type == Type::FUEL_LOSS      ? FUEL_LOSS_COLOR      :
        type == Type::CAMERA_FLIPPED ? CAMERA_FLIPPED_COLOR :
        type == Type::FORCE_SPEEDUP  ? FORCE_SPEEDUP_COLOR  :
        type == Type::FALL           ? FALL_COLOR           :
                                       SIMPLE_COLOR
    );

    SetColor(Object3D::GetColor());
    SetAlpha(Object3D::GetAlpha());

    this->type = type;
    isTouched = false;
}

Platform::Platform()
    : Platform(Type::SIMPLE)
{}

Platform* Platform::CloneImplementation() const {
    return new Platform(*this);
}

Collidable<Object3D>::Bounding::Box* Platform::GetBounding() const {
    return static_cast<Bounding::Box*>(Object3D::GetBounding());
}

bool Platform::CollidesWith(Object3D const* other) const {
    return other->GetBounding()->CollidesBox(GetBounding());
}

void Platform::CollidedWith(Object3D* other) {
    other->CollidedPlatform(this);
}

void Platform::CollidedPlayer(Player* player) {
    using namespace glm;

    if (!isTouched) {
        SetColor(TOUCHED_COLOR);
        SetTouched(true);

        if (type == Type::FUEL_GAIN || type == Type::FUEL_LOSS) {
            float level = GetScene()->GetFuelIndicator()->GetLevel();
            int dir = type == Type::FUEL_GAIN ? 1 : -1;

            GetScene()->GetFuelIndicator()->SetLevel(level + dir * FUEL_GAIN);
        }
        else if (type == Type::CAMERA_FLIPPED) {
            // ugly but cannot use covariant return on `GetCamera`
            static_cast<PlayerCamera*>(GetScene()->GetCamera())->SetFlipped(true);
        }
        else if (type == Type::FORCE_SPEEDUP) {
            GetScene()->GetPlayer()->SetColor(FORCE_SPEEDUP_COLOR);
            GetScene()->GetPlayer()->SetSpecial(true);

            GetScene()->SetSpeedup(Player::FORCE_SPEEDUP);
            GetScene()->SetSpeedupLocked(true);
        }
        else if (type == Type::FALL) {
            SetActive(false);
        }
    }

    if (GetScene()->GetFuelIndicator()->GetLevel() <= epsilon<float>() && IsActive()) {
        SetActive(false);
    }
}

void Platform::UpdateActive(float f, int mods) {
    using namespace Easing;

    Object3D::UpdateActive(f, mods);

    if (IsActive()) {
        if (GetPosition().z / ceil(SIZE.z) >= COLLISIONS_ENABLE_ROW && !HasCollisisons()) {
            SetCollisions(true);
        }
        else if (GetPosition().z / ceil(SIZE.z) >= COLLISIONS_DISABLE_ROW && HasCollisisons()) {
            SetCollisions(false);
        }

        Object3D::SetColor(Lerp(Object3D::GetColor(), color, f * 0.1f));
        Object3D::SetAlpha(Lerp(Object3D::GetAlpha(), alpha, f * 0.1f));

        SetVelocity({ 0, 0, GetScene()->GetSpeedup() * GetScene()->GetSpeed() });
    }
}

Playground* Platform::GetScene() const {
    return static_cast<Playground*>(Object3D::GetScene());
}

glm::vec3 Platform::GetColor() const {
    return color;
}

void Platform::SetColor(glm::vec3 color) {
    this->color = color;
}

float Platform::GetAlpha() const {
    return alpha;
}

void Platform::SetAlpha(float alpha) {
    this->alpha = alpha;
}

void Platform::UpdatePhysics(float f, int mods) {
    if (IsActive()) {
        if (HasPhysics()) {
            SetPosition(GetPosition() + f * GetVelocity());
        }
    }
    else {
        Object3D::UpdatePhysics(f, mods); // TODO platform type that falls when touched
    }
}

Platform::Type Platform::GetType() const {
    return type;
}

bool Platform::IsTouched() const {
    return isTouched;
}

void Platform::SetTouched(bool touched) {
    this->isTouched = touched;
}
