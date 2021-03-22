#include "FuelGauge.h"

#include "Easing.h"
#include "Player.h"
#include "Playground.h"
#include "Transform3D.h"

using StylizedRunner::FuelGauge;
using StylizedRunner::Player;
using StylizedRunner::Playground;

FuelGauge::FuelGauge() {
    using namespace Transform3D;

    SetGui(true);
    SetShader("Renderable");

    components.resize(COMPONENT_COUNT);

    components[INDICATOR] = { "Box", Translate({ 0, 0, -1 }) * Scale(SIZE), mat4 { 1 } };

    SetPosition({ 0, -0.9f, 2 });

    SetColor(Object3D::GetColor());
    SetAlpha(Object3D::GetAlpha());

    SetLevel(1);
}

void FuelGauge::UpdateActive(float dt, float f, int mods) {
    using namespace Easing;
    using namespace Transform3D;

    Object3D::UpdateActive(dt, f, mods);

    if (IsActive()) {
        SetScale({ Lerp(GetScale().x, level, f * 0.05f), 1, 1 });

        Object3D::SetColor(Lerp(Object3D::GetColor(), color, f * 0.05f));
        Object3D::SetAlpha(Lerp(Object3D::GetAlpha(), alpha, f * 0.05f));
    }
}

FuelGauge* FuelGauge::CloneImplementation() const {
    return new FuelGauge(*this);
}

std::unique_ptr<FuelGauge> FuelGauge::Clone() const {
    return std::unique_ptr<FuelGauge> { static_cast<FuelGauge*>(Object3D::Clone().release()) };
}

bool FuelGauge::CollidesWith(Object3D const* other) const {
    return false;
}

void FuelGauge::CollidedWith(Object3D* other) {
    other->CollidedFuelGauge(this);
}

Playground* FuelGauge::GetScene() const {
    return static_cast<Playground*>(Object3D::GetScene());
}

glm::vec3 FuelGauge::GetColor() const {
    return color;
}

void FuelGauge::SetColor(glm::vec3 color) {
    this->color = color;
}

float FuelGauge::GetAlpha() const {
    return alpha;
}

void FuelGauge::SetAlpha(float alpha) {
    this->alpha = alpha;
}

float FuelGauge::GetLevel() const {
    return level;
}

void FuelGauge::SetLevel(float level) {
    using namespace Easing;
    using namespace Transform3D;

    level = glm::clamp<float>(level, 0, 1);

    SetColor(Lerp(EMPTY_COLOR, FULL_COLOR, level));

    this->level = level;

    if (level <= epsilon<float>()) {
        level = 0;
    }
}
