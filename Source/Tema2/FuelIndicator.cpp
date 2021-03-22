#include "FuelIndicator.h"

#include "Easing.h"
#include "Platform.h"
#include "Player.h"
#include "Transform3D.h"

using Skyroads::FuelIndicator;
using Skyroads::Player;
using Skyroads::Playground;

glm::vec3 const FuelIndicator::OFFSET { 0, Player::DIAMETER, -1.5f };

glm::vec3 const FuelIndicator::FULL_COLOR { Platform::FUEL_GAIN_COLOR };
glm::vec3 const FuelIndicator::EMPTY_COLOR { Platform::FUEL_LOSS_COLOR };

FuelIndicator::FuelIndicator() {
    using namespace std::chrono;
    using namespace Transform3D;

    components.resize(COMPONENT_COUNT);

    components[INDICATOR] = { "Box", Scale(SIZE), mat4 { 1 } };

    SetColor(Object3D::GetColor());
    SetAlpha(Object3D::GetAlpha());

    SetLevel(1);
}

void FuelIndicator::UpdateActive(float f, int mods) {
    using namespace std::chrono;
    using namespace Easing;
    using namespace Transform3D;

    Object3D::UpdateActive(f, mods);

    if (IsActive()) {
        SetPosition(Lerp(
            GetPosition(),
            GetScene()->GetPlayer()->GetPosition() + OFFSET,
            f * 0.1f
        ));

        SetScale({ Lerp(GetLevel(), level, f * 0.1f), 1, 1 });

        Object3D::SetColor(Lerp(Object3D::GetColor(), color, f * 0.1f));
        Object3D::SetAlpha(Lerp(Object3D::GetAlpha(), alpha, f * 0.1f));
    }
}

FuelIndicator* FuelIndicator::CloneImplementation() const {
    return new FuelIndicator(*this);
}

bool FuelIndicator::CollidesWith(Object3D const* other) const {
    return false;
}

void FuelIndicator::CollidedWith(Object3D* other) {
    other->CollidedFuelIndicator(this);
}

Playground* FuelIndicator::GetScene() const {
    return static_cast<Playground*>(Object3D::GetScene());
}

glm::vec3 FuelIndicator::GetColor() const {
    return color;
}

void FuelIndicator::SetColor(glm::vec3 color) {
    this->color = color;
}

float FuelIndicator::GetAlpha() const {
    return alpha;
}

void FuelIndicator::SetAlpha(float alpha) {
    this->alpha = alpha;
}

float FuelIndicator::GetLevel() const {
    return GetScale().x;
}

void FuelIndicator::SetLevel(float level) {
    using namespace Easing;
    using namespace Transform3D;

    level = glm::clamp<float>(level, 0, 1);

    SetColor(Lerp(EMPTY_COLOR, FULL_COLOR, level));

    this->level = level;

    if (GetLevel() <= epsilon<float>()) {
        SetScale({ level, 1, 1 });
    }
}
