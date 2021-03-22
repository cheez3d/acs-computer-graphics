#include "Balloon.h"

#include "Playground.h"
#include "Tema3/Random.h"
#include "Transform2D.h"

using BowAndArrow::Balloon;
using BowAndArrow::Playground;

Balloon::Balloon(Type type, float radius) {
    using namespace Transform2D;

    radius = clamp(radius, 0.5f, 1.5f);

    components.resize(COMPONENT_COUNT);

    components[SHINE] = { "WhiteArc", Scale({ 0.7, 1 }), mat3 { 1 } };

    components[ELLIPSE] = {
        type == Type::RED ? "RedCircle" : "YellowCircle",
        Scale({ 0.7f, 1 }),
        mat3 { 1 },
    };

    float rotation = 0.25f * Random::Float(-quarter_pi<float>(), quarter_pi<float>());

    components[TRIANGLE] = {
        type == Type::RED ? "RedTriangle" : "YellowTriangle",
        Translate({ 0, -0.8f }) * Scale(0.5f),
        Rotate(rotation),
    };

    mat3 translation = Translate({ -0.01f, -1.2f });

    for (auto&& component : { STRING1, STRING2, STRING3, STRING4 }) {
        components[component] = { "WhiteLine", translation * Rotate(rotation) * Scale({ 1, -0.1f }), mat3 { 1 } };
        
        translation = Translate(Rotate(rotation) * vec3 { 0, -0.1f, 1 }) * translation;
        rotation = Random::Float(-quarter_pi<float>(), quarter_pi<float>());
    }

    mat3 model = Scale(0.6f * radius);

    for (auto&& [name, objectModel, userModel] : components) {
        objectModel = model * objectModel;
    }

    SetBoundsRadius((model * vec3 { 0.85f, 0, 1 }).x);

    this->type = type;
}

Balloon* Balloon::CloneImplementation() const {
    return new Balloon(*this);
}

void Balloon::UpdateAlive(float f, int mods) {
    using namespace Easing;

    if (!IsAlive()) {
        if (GetScene()->GetWorldBoundary().Contains(GetPosition())) {
            if (any(greaterThan(GetScale(), vec2 { 0.1f }))) {
                SetScale(Lerp(GetScale(), vec2 { 0.1f }, f * 0.5f));
            }
        }
        else {
            if (any(greaterThan(GetScale(), vec2 { 0 }))) {
                SetScale(Lerp(GetScale(), vec2 { 0 }, f * 0.5f));
            }
        }
    }
}

void Balloon::UpdatePhysics(float f, int mods) {
    if (IsAlive()) {
        if (HasPhysics()) {
            SetPosition(GetPosition() + f * GetVelocity());
        }
    }
    else {
        Object2D::UpdatePhysics(f, mods);
    }
}

Playground* Balloon::GetScene() const {
    return static_cast<Playground*>(Object2D::GetScene());
}

void Balloon::CollidedWith(Object2D* other) {
    other->CollidedBalloon(this);
}

void Balloon::CollidedBalloon(Balloon* balloon) {
    using namespace glm;

    vec2 d = GetPosition() - balloon->GetPosition();
    float f = (std::get<RADIUS>(GetBounds()) + std::get<RADIUS>(balloon->GetBounds()) - length(d)) / 2;
    vec2 dir = normalize(d);

    SetPosition(GetPosition() + f * dir);
    balloon->SetPosition(balloon->GetPosition() - f * dir); // TODO
}

Balloon::Type Balloon::GetType() const {
    return type;
}
