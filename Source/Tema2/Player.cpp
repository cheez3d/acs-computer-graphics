#include "Player.h"

#include "Easing.h"
#include "FuelIndicator.h"
#include "Platform.h"
#include "PlayerCamera.h"
#include "Playground.h"
#include "Scene3D.h"
#include "Transform3D.h"

using Skyroads::Collidable;
using Skyroads::Object3D;
using Skyroads::Platform;
using Skyroads::Player;
using Skyroads::Playground;

glm::vec3 const Player::POSITION_START { Player::COLUMN_START * ceil(Platform::SIZE.x), 2, 0 };

Player::Player() {
    using namespace std::chrono;
    using namespace Transform3D;

    auto bounding = std::make_unique<Bounding::Sphere>();
    bounding->SetPosition(GetBounding()->GetPosition());
    bounding->SetOffset(GetBounding()->GetOffset());
    bounding->SetSize(vec3 { DIAMETER });

    SetBounding(std::move(bounding));

    components.resize(COMPONENT_COUNT);

    components[SPHERE] = { "Sphere", Scale(DIAMETER), mat4 { 1 } };

    SetShader("Player");

    Object3D::SetColor(COLOR);

    SetColor(Object3D::GetColor());
    SetAlpha(Object3D::GetAlpha());

    column = COLUMN_START;
    lastJump = steady_clock::now();

    SetSpecial(false);
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
        dir = normalize(vec3 { 0, -1, 0 });
    }

    SetVelocity({ GetVelocity().x, 0, GetVelocity().z });
    SetPosition(GetPosition() + f * dir);
}

void Player::KeyChange(int key, int mods, bool press) {
    if (IsControllable()) {
        if (press && (key == GLFW_KEY_A || key == GLFW_KEY_D)) {
            int dir = key == GLFW_KEY_A ? -1 : 1;

            column = glm::clamp(
                column + dir,
                -Platform::COLUMN_RADIUS, Platform::COLUMN_RADIUS
            );
        }
    }
}

void Player::UpdateActive(float f, int mods) {
    using namespace std::chrono;
    using namespace Easing;

    Object3D::UpdateActive(f, mods);
    
    if (IsActive()) {
        Object3D::SetColor(Lerp(Object3D::GetColor(), color, f * 0.05f));

        if (abs(alpha - Object3D::GetAlpha()) > epsilon<float>()) {
            Object3D::SetAlpha(Lerp(Object3D::GetAlpha(), alpha, f * 0.1f));

            if (alpha == 1 && !IsVisible()) {
                SetVisible(true);
            }
        }
        else {
            if (alpha == 0 && IsVisible()) {
                SetVisible(false);
            }
        }

        if (GetPosition().y <= FALL_FIRST_PERSON_Y &&
            (GetScene()->GetCamera()->IsControllable() || 
             static_cast<PlayerCamera*>(GetScene()->GetCamera())->IsFlipped()))
        {
            KillCamera();
        }
        else if (GetPosition().y <= FALL_RESET_Y) {
            Kill();
        }

        SetPosition(Lerp(GetPosition(), vec3 { column * ceil(Platform::SIZE.x), GetPosition().y, 0 }, f * 0.075f));

        if (GetScene()->IsKeyHeld(GLFW_KEY_SPACE) &&
            (!HasPhysics() || HasPhysics() && GetScene()->GetFuelIndicator()->GetLevel() > epsilon<float>()))
        {
            auto t = GetScene()->GetTime();

            if (!HasPhysics()) {
                Reset();
            }

            if (abs(GetVelocity().y) <= glm::epsilon<float>()) {
                

                SetVelocity({ GetVelocity().x, 0.05f, GetVelocity().z });

                lastJump = t;
            }
            else if (duration_cast<duration<float>>(t - lastJump).count() <= JUMP_TIME_MAX) {
                SetVelocity(GetVelocity() + f * vec3 { 0, 0.01f, 0 });
            }
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
            float fov = clamp(GetScene()->GetSpeedup(), 0.7f, 1.6f) * Camera3D::DEFAULT_FOV;
            GetScene()->GetCamera()->SetFov(fov);
        }
    }
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

Playground* Player::GetScene() const {
    return static_cast<Playground*>(Object3D::GetScene());
}

void Player::UpdatePhysics(float f, int mods) {
    using namespace std::chrono;

    Object3D::UpdatePhysics(f, mods);

    if (HasPhysics()) {
        SetRotationX(GetRotation().x - f * (2 * GetScene()->GetSpeedup() * GetScene()->GetSpeed() / DIAMETER));
    }
}

void Player::PrepareShader(Shader* shader, glm::mat4 model, int component) const {
    Object3D::PrepareShader(shader, model, component);

    int loc;

    loc = glGetUniformLocation(shader->GetProgramID(), "Special");
    glUniform1i(loc, isSpecial);
}

int Player::GetColumn() const {
    return column;
}

bool Player::IsSpecial() const {
    return isSpecial;
}

void Player:: SetSpecial(bool special) {
    this->isSpecial = special;
}

void Player::KillCamera() {
    GetScene()->GetFuelIndicator()->SetAlpha(0);
    GetScene()->GetFuelIndicator()->SetLevel(0);

    // ugly but cannot use covariant return on `GetCamera`
    static_cast<PlayerCamera*>(GetScene()->GetCamera())->SetFlipped(false);

    GetScene()->GetCamera()->SetControllable(false);
    GetScene()->GetCamera()->SetRotation(PlayerCamera::START_ROTATION);
    GetScene()->GetCamera()->SetRadius(PlayerCamera::RADIUS_MIN);
}

void Player::Kill() {
    SetPosition(POSITION_START);
    SetVelocity({ 0, 0, 0 });
    SetColor(COLOR);
    SetPhysics(false);
    column = COLUMN_START;
    SetSpecial(false);

    GetScene()->SetPlatformAddRow(Platform::ADD_ROW_START);
    GetScene()->SetPlatformAddEmptyRowsMax(Platform::ADD_EMPTY_ROWS_START);
    GetScene()->SetSpeed(SPEED_START);

    GetScene()->RemovePlatforms();
    GetScene()->AddInitialPlatforms();

    GetScene()->SetSpeedupLocked(false);

    GetScene()->ResetTimes();
}

void Player::Reset() {
    GetScene()->SetPlatformAddRow(Platform::ADD_ROW_START);
    GetScene()->SetPlatformAddEmptyRowsMax(Platform::ADD_EMPTY_ROWS_START);
    GetScene()->SetSpeed(SPEED_START);

    GetScene()->ResetTimes();

    SetPhysics(true);

    GetScene()->GetFuelIndicator()->SetAlpha(1);
    GetScene()->GetFuelIndicator()->SetLevel(1);

    GetScene()->GetCamera()->SetControllable(true);
    GetScene()->GetCamera()->SetRadius(PlayerCamera::RADIUS_MAX);
}
