#include "Platform.h"

#include "Easing.h"
#include "FuelGauge.h"
#include "Object3D.h"
#include "Player.h"
#include "PlayerCamera.h"
#include "Playground.h"
#include "Random.h"
#include "SmoothCamera.h"
#include "Transform3D.h"

using StylizedRunner::Collidable;
using StylizedRunner::Object3D;
using StylizedRunner::Platform;
using StylizedRunner::Playground;

Platform::Type Platform::GetRandomType() {
    float p = 100 * Random::Float();

    return p <= 70    ? Type::SIMPLE        :
           p <= 85    ? Type::FUEL_GAIN     :
           p <= 90    ? Type::FUEL_LOSS     :
           p <= 95    ? Type::FALL          :
           p <= 97.5f ? Type::FORCE_SPEEDUP :
                        Type::CAMERA_FLIP;
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

    SetDrag(0.2f);

    SetTexture("Platform");

    Object3D::SetColor(
        type == Type::FUEL_GAIN     ? FUEL_GAIN_COLOR      :
        type == Type::FUEL_LOSS     ? FUEL_LOSS_COLOR      :
        type == Type::CAMERA_FLIP   ? CAMERA_FLIPPED_COLOR :
        type == Type::FORCE_SPEEDUP ? FORCE_SPEEDUP_COLOR  :
        type == Type::FALL          ? FALL_COLOR           :
                                      SIMPLE_COLOR
    );

    SetColor(Object3D::GetColor());
    SetAlpha(Object3D::GetAlpha());

    this->type = type;
    isTouched = false;
}

Platform::Platform() : Platform(Type::SIMPLE) {}

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
        // for pickup
        if (type == Type::SIMPLE || type == Type::FALL) {
            SetColor(TOUCHED_COLOR);
        }
        SetTouched(true);

        if (type == Type::FUEL_GAIN || type == Type::FUEL_LOSS) {
            float level = player->GetFuelGauge()->GetLevel();
            int dir = type == Type::FUEL_GAIN ? 1 : -1;

            player->GetFuelGauge()->SetLevel(level + dir * GetScene()->GetSpeed() * FUEL_GAIN);
        }
        else if (type == Type::CAMERA_FLIP) {
            player->GetCamera()->SetFlipped(true);
        }
        else if (type == Type::FORCE_SPEEDUP) {
            player->SetColor(FORCE_SPEEDUP_COLOR);
            player->SetSpecial(true);

            GetScene()->SetSpeedup(Player::FORCE_SPEEDUP);
            GetScene()->SetSpeedupLocked(true);
        }
        else if (type == Type::FALL) {
            SetActive(false);

            if (GetScene()) {
                GetScene()->soundEngine->play2D("Source/Tema3/Sounds/lavapop.ogg");
            }
        }

        // for pickup
        if (type != Type::SIMPLE && type != Type::FALL) {
            SetCollisions(false);
            SetAlpha(0);

            auto&& column = GetScene()->GetPlatforms()[COLUMN_RADIUS + player->GetColumn()];
            auto it = std::find(column.begin(), column.end(), this);

            if (it != column.end() && ++it != column.end()) {
                (*it)->SetLight(GetLight());
                (*it)->SetColor(GetColor());
                (*it)->SetTouched(true);
                SetLight(nullptr);
            }

            if (GetScene()) {
                GetScene()->soundEngine->play2D("Source/Tema3/Sounds/pop.ogg");
            }
        }
    }

    if (player->GetFuelGauge()->GetLevel() <= epsilon<float>() && IsActive()) {
        SetActive(false);
    }
}

void Platform::UpdateActive(float dt, float f, int mods) {
    using namespace Easing;

    Object3D::UpdateActive(dt, f, mods);

    // ugly hack for SetActive(false) falling gimmick (we are calling these before check for isActive)
    Object3D::SetColor(Lerp(Object3D::GetColor(), color, f * 0.1f));
    Object3D::SetAlpha(Lerp(Object3D::GetAlpha(), alpha, f * 0.1f));

    if (!IsActive()) return;

    int row = static_cast<int>(GetPosition().z / ceil(SIZE.z));

    if (!HasCollisions() &&
        abs(0 - GetAlpha()) >= epsilon<float>() && // temp for pickups to lose collisions on touch
        row >= COLLISIONS_ENABLE_ROW && row < COLLISIONS_DISABLE_ROW)
    {
        SetCollisions(true);
    }
    else if (HasCollisions() && row >= COLLISIONS_DISABLE_ROW) {
        SetCollisions(false);
    }

    if (type != Type::SIMPLE && type != Type::FALL) {
        SetRotationY(GetRotation().y + f * 0.05f);
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

void Platform::UpdatePhysics(float dt, float f, int mods) {
    if (!HasPhysics()) {
        return;
    }

    SetVelocity({ GetVelocity().x, GetVelocity().y, GetScene()->GetSpeedup() * GetScene()->GetSpeed() });

    if (IsActive()) {
        SetPosition(GetPosition() + dt * GetVelocity());
    }
    else {
        Object3D::UpdatePhysics(dt, f, mods);
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
