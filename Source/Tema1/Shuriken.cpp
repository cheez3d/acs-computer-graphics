#include "Shuriken.h"

#include "Balloon.h"
#include "Bow.h"
#include "Tema3/Random.h"
#include "Scene2D.h"
#include "Transform2D.h"

using BowAndArrow::Shuriken;
using BowAndArrow::Playground;

Shuriken::Shuriken(float bladeSize, float holeSize, float holeOutlineSize) {
    using namespace Transform2D;

    bladeSize = clamp<float>(bladeSize, 0.5f, 1);
    holeSize = clamp<float>(holeSize, 0, bladeSize);
    holeOutlineSize = clamp<float>(holeOutlineSize, 0, 0.1f);

    components.resize(COMPONENT_COUNT);

    components[BLADE_NE] = { "DarkGrayTriangle", Translate((bladeSize / 2) * vec2 { -1, 1 }), mat3 { 1 } };

    for (auto&& component : { BLADE_NW, BLADE_SW, BLADE_SE }) {
        components[component] = {
            std::get<NAME>(components[BLADE_NE]),
            Rotate(component * half_pi<float>()) * std::get<OBJECT_MODEL>(components[BLADE_NE]),
            std::get<USER_MODEL>(components[BLADE_NE]),
        };
    }

    components[HOLE_OUTLINE_NE] = {
        "BlackSquare",
        Translate((holeSize / 2) * vec2 { -1, 1 }) *
        Scale(holeSize * vec2 { 1, 0 } + holeOutlineSize * vec2 { 1, 1 }),
        mat3 { 1 },
    };

    for (auto&& component : { HOLE_OUTLINE_NW, HOLE_OUTLINE_SW, HOLE_OUTLINE_SE }) {
        components[component] = {
            std::get<NAME>(components[HOLE_OUTLINE_NE]),
            Rotate(component * half_pi<float>()) * std::get<OBJECT_MODEL>(components[HOLE_OUTLINE_NE]),
            std::get<USER_MODEL>(components[HOLE_OUTLINE_NE]),
        };
    }

    components[HOLE_FILLER_NE] = {
        "DarkGraySquare",
        Translate((holeSize / 2) * vec2 { -1, 1 }) *
        Scale((bladeSize / 2) * vec2 { 1, 1 } + (holeSize / 2) * vec2 { 1, -1 }),
        mat3 { 1 },
    };

    for (auto&& component : { HOLE_FILLER_NW, HOLE_FILLER_SW, HOLE_FILLER_SE }) {
        components[component] = {
            std::get<NAME>(components[HOLE_FILLER_NE]),
            Rotate(component * half_pi<float>()) * std::get<OBJECT_MODEL>(components[HOLE_FILLER_NE]),
            std::get<USER_MODEL>(components[HOLE_FILLER_NE]),
        };
    }

    mat3 model = Scale(0.4f);

    for (auto&& [name, objectModel, userModel] : components) {
        objectModel = model * objectModel;
    }

    SetBoundsRadius((model * (vec3 { 1.0f / 2 + bladeSize + 1.0f / 2, 0, 1 } / 2.0f)).x);
}

Shuriken* Shuriken::CloneImplementation() const {
    return new Shuriken(*this);
}

void Shuriken::UpdateAlive(float f, int mods) {
    using namespace glm;

    Object2D::UpdateAlive(f, mods);

    if (!IsAlive()) {
        if (GetScene()->GetWorldBoundary().Contains(GetPosition()) &&
            any(greaterThanEqual(GetScale(), vec2 { 1 - epsilon<float>() })))
        {
            vec2 d = GetScene()->GetBow()->GetPosition() - GetPosition();
            float vx = (1 + 0.5f * (GetScene()->GetScore() / 1000.0f) ) * Random::Float(-0.2f, -0.4f);
            float t = d.x / vx;
            float vy = d.y / t - 0.5f * GetScene()->GetGravity() * t;

            SetVelocity({ vx, vy });

            SetAlive(true);
            SetPhysics(true);
        }
    }
}

void Shuriken::UpdatePhysics(float f, int mods) {
    glm::vec2 position = this->GetPosition();

    Object2D::UpdatePhysics(f, mods);

    if (HasPhysics()) {
        float d = glm::length(this->GetPosition() - position);

        SetRotation(GetRotation() + f * (d / std::get<RADIUS>(GetBounds())));
    }
}

Playground* Shuriken::GetScene() const {
    return static_cast<Playground*>(Object2D::GetScene());
}

void Shuriken::CollidedWith(Object2D* other) {
    other->CollidedShuriken(this);
}

void Shuriken::CollidedBalloon(Balloon* balloon) {
    balloon->SetAlive(false);
    balloon->SetCollisions(false);
}
