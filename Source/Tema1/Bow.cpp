#include "Bow.h"

#include "Tema3/Easing.h"
#include "Playground.h"
#include "Shuriken.h"
#include "Transform2D.h"

using BowAndArrow::Arrow;
using BowAndArrow::Bow;
using BowAndArrow::Playground;

Bow::Bow() {
    using namespace std::chrono;
    using namespace Transform2D;

    components.resize(COMPONENT_COUNT);

    components[ARC] = { "BrownArc", mat3 { 1 }, mat3 { 1 } };
    components[UPPER_STRING] = { "WhiteLine", Translate({ 0, 1 }) * Scale({ 1, -1 }), mat3 { 1 } };
    components[LOWER_STRING] = { "WhiteLine", Translate({ 0, -1 }), mat3 { 1 } };

    for (auto&& [name, objectModel, userModel] : components) {
        objectModel = Translate({ -0.25f, 0 }) * objectModel;
    }

    AddAttachment(ARROW, std::make_unique<Arrow>());
    attachments[ARROW]->SetPosition({ (1 - 0.5f) / 2, 0 });

    SetBoundsRadius(1 / sqrt(2.0f));

    SetLoaded(true);
    SetPower(0);
    lastShot = steady_clock::now()
             - duration_cast<steady_clock::duration>(duration<float>(SHOT_DELAY));
}

Bow* Bow::CloneImplementation() const {
    return new Bow(*this);
}

void Bow::UpdateAlive(float f, int mods) {
    using namespace std::chrono;
    using namespace Easing;

    Object2D::UpdateAlive(f, mods);

    if (IsAlive()) {
        if (GetScene()->IsKeyHeld(GLFW_KEY_W) || GetScene()->IsKeyHeld(GLFW_KEY_S)) {
            int dir = GetScene()->IsKeyHeld(GLFW_KEY_W) ? 1 : -1;

            float d = 0.15f;

            if (mods & GLFW_MOD_SHIFT) {
                d *= 2;
            }

            if (mods & GLFW_MOD_ALT) {
                d /= 2;
            }

            elevation += f * dir * d;

            elevation = clamp(
                elevation,
                GetScene()->GetWorld().GetMin().y + 1,
                GetScene()->GetWorld().GetMax().y - 1
            );
        }

        SetPosition(Lerp(GetPosition(), { GetPosition().x, elevation }, f * 0.1f));

        vec2 bowToMouse = GetScene()->GetMouse() - GetPosition();

        float phi = clamp(
            atan(bowToMouse.y, bowToMouse.x),
            -1.75f * quarter_pi<float>(),
            1.75f * quarter_pi<float>()
        );

        SetRotation(Lerp(GetRotation(), phi, f * 0.1f));

        if (GetScene()->IsMouseHeld(GLFW_MOUSE_BUTTON_LEFT) && isLoaded) {
            SetPower(Lerp(power, 1, f * (mods & GLFW_MOD_ALT ? 0.5f : 1) * 0.025f));
        }
        else {
            SetPower(Lerp(power, 0, f * 0.5f));
        }

        if (!isLoaded) {
            auto t = GetScene()->GetTime();

            if (duration_cast<duration<float>>(t - lastShot).count() >= SHOT_DELAY) {
                SetLoaded(true);
            }
        }
    }
}

void Bow::MouseChange(int btn, int mods, bool press) {
    using namespace std::chrono;

    if (btn == GLFW_MOUSE_BUTTON_1 && !press && IsAlive() && isLoaded) {
        GetScene()->AddObject(SetLoaded(false));
        lastShot = GetScene()->GetTime();
    }
}

Playground* Bow::GetScene() const {
    return static_cast<Playground*>(Object2D::GetScene());
}

void Bow::CollidedWith(Object2D* other) {
    other->CollidedBow(this);
}

void Bow::CollidedShuriken(Shuriken* shuriken) {
    this->SetCollisions(false);
    shuriken->SetCollisions(false);

    GetScene()->ShowDamage();
    GetScene()->TakeHeart();
    GetScene()->AddScore(-5);
}

bool Bow::IsLoaded() const {
    return isLoaded;
}

std::unique_ptr<Arrow> Bow::SetLoaded(bool loaded) {
    using namespace Transform2D;

    std::unique_ptr<Arrow> arrow { nullptr };

    isLoaded = loaded;

    if (!loaded) {
        arrow = std::unique_ptr<Arrow> { static_cast<Arrow*>(CloneAttachment(ARROW).release()) };

        arrow->SetPhysics(true);
        arrow->SetVelocity(pow(power, 2) * Rotate(arrow->GetRotation()) * vec3 { 1, 0, 1 });
        arrow->SetPower(0);
    }

    attachments[ARROW]->SetVisible(loaded);

    return arrow;
}

float Bow::GetPower() const {
    return power;
}

void Bow::SetPower(float power) {
    using namespace Transform2D;

    this->power = power;

    float f = 0.5f * power;

    float phi = atan<float>(f, 1);
    float s = sqrt(f * f + 1);

    std::get<USER_MODEL>(components[UPPER_STRING]) = Rotate(phi) * Scale({ 1, s });
    std::get<USER_MODEL>(components[LOWER_STRING]) = Rotate(phi) * Scale({ 1, s });

    Arrow* arrow = static_cast<Arrow*>(attachments[ARROW].get());

    arrow->SetOffset({ -f, 0 });
    arrow->SetPower(power);
}
