#include "Playground.h"

#include "Easing.h"
#include "FuelIndicator.h"
#include "Platform.h"
#include "Player.h"
#include "PlayerCamera.h"
#include "Random.h"

using Skyroads::FuelIndicator;
using Skyroads::Platform;
using Skyroads::Player;
using Skyroads::PlayerCamera;
using Skyroads::Playground;

void Playground::Init() {
    Scene3D::Init();

    auto player = std::make_unique<Player>();
    player->SetPosition(Player::POSITION_START);
    this->player = player.get();
    AddObject(std::move(player));

    auto fuelIndicator = std::make_unique<FuelIndicator>();
    this->fuelIndicator = fuelIndicator.get();
    AddObject(std::move(fuelIndicator));

    auto camera = std::make_unique<PlayerCamera>();
    camera->SetTarget(this->player);
    SetCamera(std::move(camera));
    this->player->KillCamera();

    Scene3D::SetLight({ this->player->GetPosition().x, LIGHT_Y, this->player->GetPosition().z });
    SetLight(Scene3D::GetLight());

    platforms.resize(2 * Platform::COLUMN_RADIUS + 1);
    platformAddRow = Platform::ADD_ROW_START;
    platformAddShifts.resize(platforms.size());
    platformAddEmptyRows.resize(platforms.size());
    platformAddEmptyRowsMax = Platform::ADD_EMPTY_ROWS_START;

    AddInitialPlatforms();

    SetSpeed(Player::SPEED_START);
    SetSpeedup(1);
    SetSpeedupLocked(false);

    ResetTimes();
}

void Playground::OnInputUpdate(float dt, int mods) {
    using namespace std::chrono;
    using namespace Easing;

    Scene3D::OnInputUpdate(dt, mods);

    float f = dt / (1.0f / 60);
    auto t = GetTime();

    if (!IsPaused()) {
        SetLight({ player->GetPosition().x, LIGHT_Y, player->GetPosition().z });
        Scene3D::SetLight(Lerp(Scene3D::GetLight(), light, f * 0.005f));

        for (int c = -Platform::COLUMN_RADIUS; c <= Platform::COLUMN_RADIUS; ++c) {
            int i = c + Platform::COLUMN_RADIUS;
            auto&& column = platforms[i];

            float x = c * ceil(Platform::SIZE.x);

            float removeZ = Platform::REMOVE_ROW * ceil(Platform::SIZE.z);

            if (!column.empty() && column.back()->GetPosition().z >= removeZ) {
                RemoveObject(column.back());
                column.pop_back();
            }

            for (auto p = column.rbegin(); p != column.rend(); ++p) {
                int r = static_cast<int>((*p)->GetPosition().z / ceil(Platform::SIZE.z));

                if (r == Platform::FADE_ROW) {
                    (*p)->SetAlpha(0);
                    break;
                }
            }

            float addZ = platformAddRow * ceil(Platform::SIZE.z) + platformAddShifts[i];

            if (column.empty() ||
                abs(addZ - column.front()->GetPosition().z) >= (1 + platformAddEmptyRows[i]) * ceil(Platform::SIZE.z))
            {
                AddPlatform(c, platformAddRow);
                column.front()->Object3D::SetAlpha(0);
            }
        }

        if (platformAddRow > Platform::ADD_ROW_MAX &&
            duration_cast<duration<float>>(t - lastPlatformAddRowIncrement).count() >= Platform::ADD_ROW_INCREMENT_DELAY)
        {
            SetPlatformAddRow(platformAddRow - 1);
            lastPlatformAddRowIncrement = t;
        }

        if (platformAddEmptyRowsMax < Platform::ADD_EMPTY_ROWS_MAX &&
            duration_cast<duration<float>>(t - lastPlatformAddEmptyRowsMaxIncrement).count() >= Platform::ADD_EMPTY_ROWS_INCREMENT_DELAY)
        {
            SetPlatformAddEmptyRowsMax(platformAddEmptyRowsMax + 1);
            lastPlatformAddEmptyRowsMaxIncrement = t;
        }

        if (speed < Player::SPEED_MAX &&
            duration_cast<duration<float>>(t - lastSpeedIncrement).count() >= Player::SPEED_INCREMENT_DELAY)
        {
            SetSpeed((glm::min)(speed + Player::SPEED_INCREMENT, Player::SPEED_MAX));
            lastSpeedIncrement = t;
        }

        if (isSpeedupLocked &&
            duration_cast<duration<float>>(t - lastSpeedupLock).count() >= Player::FORCE_SPEEDUP_DURATION)
        {
            SetSpeedupLocked(false);

            player->SetColor(Player::COLOR);
            player->SetSpecial(false);
        }

        if (fuelIndicator->GetLevel() > epsilon<float>() &&
            duration_cast<duration<float>>(t - lastFuelIndicatorLevelDecrement).count() >= FuelIndicator::LEVEL_DECREMENT_DELAY)
        {
            fuelIndicator->SetLevel(fuelIndicator->GetLevel() - speedup * FuelIndicator::LEVEL_DECREMENT);
            lastFuelIndicatorLevelDecrement = t;
        }
    }
}

glm::vec3 Playground::GetLight() const {
    return light;
}

void Playground::SetLight(glm::vec3 light) {
    this->light = light;
}

Player* Playground::GetPlayer() const {
    return player;
}

FuelIndicator* Skyroads::Playground::GetFuelIndicator() const {
    return fuelIndicator;
}

std::vector<std::list<Platform*>>& Playground::GetPlatforms() {
    return platforms;
}

void Playground::AddInitialPlatforms() {
    using namespace Easing;

    for (int i = 0; i < platforms.size(); ++i) {
        platformAddShifts[i] = Lerp(0, ceil(Platform::SIZE.z), static_cast<float>(i) / (platforms.size() - 1));
    }

    for (int c = -Platform::COLUMN_RADIUS; c <= Platform::COLUMN_RADIUS; ++c) {
        int i = c + Platform::COLUMN_RADIUS;
        auto&& column = platforms[i];

        float x = c * ceil(Platform::SIZE.x);

        for (int r = 0; r >= platformAddRow; ) {
            AddPlatform(c, r);

            r -= 1 + platformAddEmptyRows[i];
        }
    }
}

void Playground::RemovePlatforms() {
    for (int c = -Platform::COLUMN_RADIUS; c <= Platform::COLUMN_RADIUS; ++c) {
        int i = c + Platform::COLUMN_RADIUS;
        auto&& column = platforms[i];

        while (!column.empty()) {
            RemoveObject(column.back());
            column.pop_back();
        }
    }
}

void Playground::SetPlatformAddRow(int platformAddRow) {
    this->platformAddRow = platformAddRow;
}

void Playground::SetPlatformAddEmptyRowsMax(int platformAddEmptyRowsMax) {
    this->platformAddEmptyRowsMax = platformAddEmptyRowsMax;
}

void Playground::AddPlatform(int column, int row) {
    int i = column + Platform::COLUMN_RADIUS;

    float x = column * ceil(Platform::SIZE.x);
    float z = row * ceil(Platform::SIZE.z) + platformAddShifts[i];

    auto platform = std::make_unique<Platform>(Platform::GetRandomType());
    platform->SetCollisions(false);
    platform->SetPosition({ x, 0, z });
    platform->SetVelocity({ 0, 0, speedup * speed });
    platform->SetPhysics(true);

    platforms[i].push_front(platform.get());
    AddObject(std::move(platform));

    platformAddEmptyRows[i] = Random::Int(0, static_cast<int>(glm::round(platformAddEmptyRowsMax)));

    if (std::all_of(platformAddEmptyRows.begin(), platformAddEmptyRows.end(), [](int rs) { return rs > 1; })) {
        platformAddEmptyRows[i] = Random::Int(0, 1);
    }
}

float Playground::GetSpeed() const {
    return speed;
}

void Playground::SetSpeed(float speed) {
    this->speed = speed;
}

float Playground::GetSpeedup() const {
    return speedup;
}

void Playground::SetSpeedup(float speedup) {
    this->speedup = speedup;
}

bool Playground::IsSpeedupLocked() const {
    return isSpeedupLocked;
}

void Playground::SetSpeedupLocked(bool speedupLocked) {
    this->isSpeedupLocked = speedupLocked;

    if (speedupLocked) {
        lastSpeedupLock = GetTime();
    }
}

void Playground::ResetTimes() {
    lastFuelIndicatorLevelDecrement = GetTime();
    lastPlatformAddRowIncrement = GetTime();
    lastPlatformAddEmptyRowsMaxIncrement = GetTime();
    lastSpeedIncrement = GetTime();
    lastSpeedupLock = GetTime();

    // ugly but cannot use covariant return on `GetCamera`
    static_cast<PlayerCamera*>(GetCamera())->ResetTimes();
}
