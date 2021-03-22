#include "Arrow.h"

#include "Balloon.h"
#include "Playground.h"
#include "Shuriken.h"
#include "Transform2D.h"

using BowAndArrow::Arrow;
using BowAndArrow::Playground;

Arrow::Arrow() {
    using namespace Transform2D;

    components.resize(COMPONENT_COUNT);

    components[TIP] = { "DarkGrayPointer", Translate({ 1, 0 }), mat3 { 1 } };
    components[BACK] = { "DarkGrayPointer", Translate({ -0.05f, 0 }) * Scale({ 0.25f ,0.6f }), mat3 { 1 } };
    components[POWER_BAR] = { "DarkRedLine", mat3 { 1 }, mat3 { 1 } };
    components[STICK] = { "OrangeLine", mat3 { 1 }, mat3 { 1 } };

    mat3 model = Translate({ -0.5f, 0 });

    for (auto&& [name, objectModel, userModel] : components) {
        objectModel = model * objectModel;
    }

    SetBounds({ std::get<OBJECT_MODEL>(components[TIP]) * vec3 { 0, 0, 1 } , sqrt(2 * 0.1f * 0.1f) });

    SetPower(0);
}

Arrow* Arrow::CloneImplementation() const {
    return new Arrow(*this);
}

void Arrow::UpdatePhysics(float f, int mods) {
    Object2D::UpdatePhysics(f, mods);

    if (HasPhysics()) {
        SetRotation(glm::atan(GetVelocity().y, GetVelocity().x));
    }
}

Playground* Arrow::GetScene() const {
    return static_cast<Playground*>(Object2D::GetScene());
}

void Arrow::CollidedWith(Object2D* other) {
    other->CollidedArrow(this);
}

void Arrow::CollidedBalloon(Balloon* balloon) {
    balloon->SetAlive(false);
    balloon->SetCollisions(false);

    if (GetScene()->GetWorld().Contains(balloon->GetPosition())) {
        GetScene()->AddScore(balloon->GetType() == Balloon::Type::RED ? 10 : -5);
    }
}

void Arrow::CollidedShuriken(Shuriken* shuriken) {
    if (!shuriken->IsAlive()) {
        return;
    }

    shuriken->SetPhysics(false);
    shuriken->SetCollisions(false);

    SetVelocity((3.0f * GetVelocity() + shuriken->GetVelocity()) / 4.0f);

    AddAttachment(static_cast<int>(attachments.size()), GetScene()->RemoveObject(shuriken));

    GetScene()->AddScore(10);
}

void Arrow::SetPower(float power) {
    using namespace Transform2D;

    this->power = power;

    std::get<USER_MODEL>(components[POWER_BAR]) = Scale({ power, 1 });
}
