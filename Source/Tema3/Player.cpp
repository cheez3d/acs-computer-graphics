#include "Player.h"

#include "Easing.h"
#include "Platform.h"
#include "PlayerCamera.h"
#include "Playground.h"
#include "Scene3D.h"
#include "Transform3D.h"

#include <Core/Engine.h>

using StylizedRunner::Collidable;
using StylizedRunner::FuelGauge;
using StylizedRunner::Object3D;
using StylizedRunner::Platform;
using StylizedRunner::Player;
using StylizedRunner::PlayerCamera;
using StylizedRunner::Playground;

glm::vec3 const Player::POSITION_START { Player::COLUMN_START * ceil(Platform::SIZE.x), 3, 0 };

Player::Player() {
    using namespace Transform3D;

    SetCollisions(true);
    SetDynamic(true);

    auto bounding = std::make_unique<Bounding::Sphere>();
    bounding->SetPosition(GetBounding()->GetPosition());
    bounding->SetOffset(GetBounding()->GetOffset());
    bounding->SetSize(vec3 { DIAMETER });
    SetBounding(std::move(bounding));

    components.resize(COMPONENT_COUNT);
    components[SPHERE] = { "Sphere", Scale(DIAMETER), mat4 { 1 } };

    Object3D::SetColor(COLOR); SetColor(Object3D::GetColor());
    SetAlpha(Object3D::GetAlpha());

    SetShader("Player");
    SetTexture("Player");

    SetColumn(COLUMN_START);
    SetSpecial(false);
    SetFuelGauge(std::make_unique<FuelGauge>());

    jumpTimer.SetDuration(0.2f);
    fuelTimer.SetDuration(1);
}

Player::Player(Player const& other) : PlaygroundObject(other) {
    SetColor(other.GetColor());
    SetAlpha(other.GetAlpha());

    SetColumn(other.column);
    SetSpecial(other.isSpecial);
    SetFuelGauge(fuelGauge->Clone());

    jumpTimer = other.jumpTimer;
    fuelTimer = other.fuelTimer;
}

void Player::UpdateActive(float dt, float f, int mods) {
    using namespace Easing;

    Object3D::UpdateActive(dt, f, mods);

    if (!IsActive()) return;

    SetPosition(Lerp(
        GetPosition(),
        vec3 { column * ceil(Platform::SIZE.x), GetPosition().y, GetPosition().z },
        f * 0.075f
    ));

    if (GetScene()->IsKeyHeld(GLFW_KEY_SPACE)) {
        if (!HasPhysics()) {
            Reset();
        }
        else if (!jumpTimer.IsElapsed()) {
            SetVelocity(GetVelocity() + f * vec3 { 0, 0.25f, 0 });
        }
    }

    Object3D::SetColor(Lerp(Object3D::GetColor(), color, f * 0.05f));

    if (abs(alpha - Object3D::GetAlpha()) > epsilon<float>()) {
        Object3D::SetAlpha(Lerp(Object3D::GetAlpha(), alpha, f * 0.1f));

        if (!IsVisible() && abs(Object3D::GetAlpha() - 0) > epsilon<float>()) {
            SetVisible(true);
        }
    }
    else {
        if (IsVisible() &&
            abs(0 - Object3D::GetAlpha()) <= epsilon<float>())
        {
            SetVisible(false);
            Object3D::SetAlpha(0);
        }
        else if (abs(1 - Object3D::GetAlpha()) <= epsilon<float>()) {
            Object3D::SetAlpha(1);
        }
    }

    if (GetPosition().y <= FALL_FIRST_PERSON_Y &&
        (GetCamera()->IsControllable() || GetCamera()->IsFlipped()))
    {
        KillCamera();
    }
    else if (GetPosition().y <= FALL_RESET_Y) {
        Kill();
    }

    if (fuelGauge->GetLevel() >= epsilon<float>() && fuelTimer.IsElapsed()) {
        fuelGauge->SetLevel(fuelGauge->GetLevel() - GetScene()->GetSpeedup() * FuelGauge::LEVEL_DECREMENT);

        fuelTimer.SetElapsed(false);
    }

    if (!GetScene()->IsSpeedupLocked()) {
        bool w = GetScene()->IsKeyHeld(GLFW_KEY_W);
        bool s = GetScene()->IsKeyHeld(GLFW_KEY_S);

        if (w || s) {
            GetScene()->SetSpeedup(w ? SPEEDUP : 1 / SPEEDUP);
        }
        else {
            GetScene()->SetSpeedup(1);
        }
    }

    {
        float fov = clamp(GetScene()->GetSpeedup(), 0.7f, 1.6f) * 60; // TODO use `Camera3D::DEFAULT_FOV`
        GetCamera()->SetFov(fov);
    }

    fuelGauge->UpdateActive(dt, f, mods);
}

Player* Player::CloneImplementation() const {
    return new Player(*this);
}

Collidable<Object3D>::Bounding::Sphere* Player::GetBounding() const {
    return static_cast<Bounding::Sphere*>(Object3D::GetBounding());
}

bool Player::CollidesWith(Object3D const* other) const {
    return other->GetBounding()->CollidesSphere(GetBounding());
}

void Player::CollidedWith(Object3D* other) {
    other->CollidedPlayer(this);
}

void Player::CollidedPlatform(Platform* platform) {
    using namespace glm;

    // temp hack for pickups
    if (platform->GetType() != Platform::Type::SIMPLE &&
        platform->GetType() != Platform::Type::FALL)
    {
        return;
    }

    Bounding::Sphere* sphere = GetBounding();
    Bounding::Box* box = platform->GetBounding();

    vec3 position = sphere->GetModel() * vec4 { sphere->GetPosition(), 1 };
    float radius = (sphere->GetModel() * vec4 { vec3 { sphere->GetRadius() }, 0 }).x;

    vec3 boxMin = box->GetModel() * vec4 { box->GetMin(), 1 };
    vec3 boxMax = box->GetModel() * vec4 { box->GetMax(), 1 };

    vec3 p = clamp(position, boxMin, boxMax);

    vec3 d = position - p;
    float f = radius - length(d);
    vec3 dir = normalize(d);

    if (all(lessThanEqual(d, vec3 { epsilon<float>() }))) {
        dir = normalize(vec3 { 0, 1, 0 });
    }

    SetVelocity({ GetVelocity().x, platform->GetVelocity().y, GetVelocity().z });
    SetPosition(GetPosition() + f * dir);

    if (GetScene()->IsKeyHeld(GLFW_KEY_SPACE) &&
        HasPhysics() &&
        fuelGauge->GetLevel() > epsilon<float>() &&
        dot(dir, { 0, 1, 0 }) >= 0)
    {
        SetVelocity({ GetVelocity().x, 4, GetVelocity().z });

        jumpTimer.SetElapsed(false);
    }

    if (HasPhysics() && dot(dir, { 0, 1, 0 }) <= 0) {
        SetRotationX(GetRotation().x + GetScene()->dt * (2 * GetScene()->GetSpeedup() * GetScene()->GetSpeed() / DIAMETER));
    }
}

void Player::KeyChange(int key, int mods, bool press) {
    Object3D::KeyChange(key, mods, press);

    if (IsControllable()) {
        if (press && (key == GLFW_KEY_A || key == GLFW_KEY_D)) {
            int dir = key == GLFW_KEY_A ? -1 : 1;

            SetColumn(glm::clamp(column + dir, -Platform::COLUMN_RADIUS, Platform::COLUMN_RADIUS));
        }
    }
}

void Player::UpdatePhysics(float dt, float f, int mods) {
    Object3D::UpdatePhysics(dt, f, mods);

    if (!HasPhysics()) return;

    SetRotationX(GetRotation().x - dt * (2 * GetScene()->GetSpeedup() * GetScene()->GetSpeed() / DIAMETER));
}

void Player::Render(float dt, float f, int mods) {
    Object3D::Render(dt, f, mods);

    fuelGauge->Render(dt, f, mods);
}

glm::vec3 Player::GetColor() const {
    return color;
}

void Player::SetColor(glm::vec3 color) {
    this->color = color;
}

float Player::GetAlpha() const {
    return alpha;
}

void Player::SetAlpha(float alpha) {
    this->alpha = alpha;
}

PlayerCamera* Player::GetCamera() const {
    return static_cast<PlayerCamera*>(Object3D::GetCamera());
}

void Player::SetCamera(PlayerCamera* camera) {
    Object3D::SetCamera(camera);
}

Playground* Player::GetScene() const {
    return static_cast<Playground*>(Object3D::GetScene());
}

void Player::SetScene(Scene3D* scene) {
    Object3D::SetScene(scene);

    fuelGauge->SetScene(scene);

    auto watch = scene ? scene->GetWatch() : nullptr;

    jumpTimer.SetWatch(watch);
    fuelTimer.SetWatch(watch);
}

void Player::SetScene(Playground* scene) {
    SetScene(static_cast<Scene3D*>(scene));
}

void Player::UpdateShader(Shader* shader, glm::mat4 model, int component) const {
    Object3D::UpdateShader(shader, model, component);

    int loc;

    loc = glGetUniformLocation(shader->GetProgramID(), "IsSpecial");
    glUniform1i(loc, isSpecial);
}

int Player::GetColumn() const {
    return column;
}

void Player::SetColumn(int column) {
    this->column = column;
}

bool Player::IsSpecial() const {
    return isSpecial;
}

void Player:: SetSpecial(bool special) {
    this->isSpecial = special;

    // temp TODO
    if (GetScene()) {
        GetScene()->SetGravity(special ? glm::vec3 { 0, -2.5f, 0 } : glm::vec3 { 0, -10, 0 });
    }
}

FuelGauge* Player::GetFuelGauge() const {
    return fuelGauge.get();
}

void Player::SetFuelGauge(std::unique_ptr<FuelGauge> fuelGauge) {
    this->fuelGauge = std::move(fuelGauge);
}

void Player::KillCamera() {
    fuelGauge->SetAlpha(0);
    fuelGauge->SetLevel(0);

    // !! TODO (currently called before SetControllable because SetFlipped also alters controllable flag)
    GetCamera()->SetFlipped(false);

    GetCamera()->SetControllable(false);
    GetCamera()->SetRotation(PlayerCamera::START_ROTATION);
    GetCamera()->SetRadius(PlayerCamera::RADIUS_MIN);

    static bool firstTime = true;
    if (GetScene() && !firstTime) {

        irrklang::ISound* hurt = GetScene()->soundEngine->play2D("Source/Tema3/Sounds/classic_hurt.ogg", false, false, true);
        hurt->setVolume(0.25f);
    }
    else {
        firstTime = false;
    }
}

void Player::Kill() {
    SetPosition(POSITION_START);
    SetVelocity({ 0, 0, 0 });
    SetColor(COLOR);
    SetPhysics(false);
    SetColumn(COLUMN_START);
    SetSpecial(false);

    GetScene()->SetPlatformAddRow(Platform::ADD_ROW_START);
    GetScene()->SetPlatformAddEmptyRowsMax(Platform::ADD_EMPTY_ROWS_START);
    GetScene()->SetSpeed(SPEED_START);

    GetScene()->RemovePlatforms();

    GetScene()->SetSpeedupLocked(false);

    GetScene()->ResetTimes();
}

void Player::Reset() {
    GetScene()->SetPlatformAddRow(Platform::ADD_ROW_START);
    GetScene()->SetPlatformAddEmptyRowsMax(Platform::ADD_EMPTY_ROWS_START);
    GetScene()->SetSpeed(SPEED_START);

    GetScene()->ResetTimes();

    SetPhysics(true);

    GetCamera()->SetControllable(true);
    GetCamera()->SetRadius(PlayerCamera::RADIUS_MAX);

    fuelGauge->SetAlpha(1);
    fuelGauge->SetLevel(1);

    fuelTimer.SetElapsed(false);
}
