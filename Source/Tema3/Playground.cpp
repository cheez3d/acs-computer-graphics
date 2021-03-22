#include "Playground.h"

#include "Easing.h"
#include "Platform.h"
#include "PlayerCamera.h"
#include "PointLight.h"
#include "Random.h"
#include "SpotLight.h"

using StylizedRunner::Platform;
using StylizedRunner::Player;
using StylizedRunner::PlayerCamera;
using StylizedRunner::Playground;
using StylizedRunner::PlaygroundObject;

using namespace irrklang;

Playground::Playground() {
    using namespace Easing;

    soundEngine = std::unique_ptr<ISoundEngine> { createIrrKlangDevice() };
    musicTimer.SetElapsed(true);

    {
        ISound* lava = soundEngine->play2D("Source/Tema3/Sounds/lava.ogg", true, false, true);
        lava->setVolume(0.25f);
    }

    {
        auto camera = std::make_unique<PlayerCamera>();
        SetCamera(std::move(camera));
    }

    {
        auto player = std::make_unique<Player>();
        auto light = std::make_unique<SpotLight>();
        player->SetPosition(Player::POSITION_START);
        player->SetCamera(GetCamera());
        player->SetLight(light.get());
        AddLight(std::move(light));
        SetPlayer(std::move(player));
    }

    platforms.resize(2 * Platform::COLUMN_RADIUS + 1);
    platformAddRow = Platform::ADD_ROW_START;
    platformAddShifts.resize(platforms.size());
    platformAddEmptyRows.resize(platforms.size());
    platformAddEmptyRowsMax = Platform::ADD_EMPTY_ROWS_START;

    for (int i = 0; i < platforms.size(); ++i) {
        platformAddShifts[i] = Lerp(0, ceil(Platform::SIZE.z), (float)i / (platforms.size() - 1));
    }

    SetSpeed(Player::SPEED_START);
    SetSpeedup(1);
    SetSpeedupLocked(false);

    player->KillCamera();
    player->Kill();

    platformAddRowTimer.SetDuration(7.5f);
    platformAddEmptyRowsMaxTimer.SetDuration(15);

    speedIncrementTimer.SetDuration(5);
    speedupLockTimer.SetDuration(5);
}

void Playground::Init() {
    Scene3D::Init();

    musicTimer.SetWatch(GetWatch());

    platformAddRowTimer.SetWatch(GetWatch());
    platformAddEmptyRowsMaxTimer.SetWatch(GetWatch());

    speedIncrementTimer.SetWatch(GetWatch());
    speedupLockTimer.SetWatch(GetWatch());
}
#include <iostream>
void Playground::UpdateActive(float dt, float f, int mods) {
    using namespace Easing;

    Scene3D::UpdateActive(dt, f, mods);

    if (musicTimer.IsElapsed()) { // NB does not elapse at the right time after pausing the game
        char const* tracks[] = {
            "Source/Tema3/Sounds/chrysopoeia.ogg",
            "Source/Tema3/Sounds/nether1.ogg",
            "Source/Tema3/Sounds/nether2.ogg",
            "Source/Tema3/Sounds/nether3.ogg",
            "Source/Tema3/Sounds/nether4.ogg",
            "Source/Tema3/Sounds/rubeo.ogg",
            "Source/Tema3/Sounds/so_below.ogg",
        };

        int i = Random::Int(0, sizeof(tracks) / sizeof(tracks[0]) - 1);
        ISound* music = soundEngine->play2D(tracks[i], false, false, true);
        music->setVolume(0 /* 0.6f */);

        musicTimer.SetDuration(music->getPlayLength() / 1000.0f);
        musicTimer.SetElapsed(false);
    }

    if (IsPaused()) return;

    if (!Activable::IsActive()) return;

    for (int c = -Platform::COLUMN_RADIUS; c <= Platform::COLUMN_RADIUS; ++c) {
        int i = c + Platform::COLUMN_RADIUS;
        auto&& column = platforms[i];

        float x = c * ceil(Platform::SIZE.x);

        float removeZ = Platform::REMOVE_ROW * ceil(Platform::SIZE.z);

        if (!column.empty() && column.back()->GetPosition().z >= removeZ) {
            RemoveObject(column.back());
            column.pop_back();
        }

        for (auto&& it = column.rbegin(); it != column.rend(); ++it) {
            auto platform = *it;

            int row = static_cast<int>(platform->GetPosition().z / ceil(Platform::SIZE.z));

            if (row >= Platform::FADE_ROW) {
                platform->SetAlpha(0);

                //break; // do not break in order to lerp alpha of obstacles TODO
            }
        }

        float addZ = platformAddRow * ceil(Platform::SIZE.z) + platformAddShifts[i];

        int rowOffset = 1 + platformAddEmptyRows[i];
        float offset = rowOffset * ceil(Platform::SIZE.z);

        if (column.empty() || abs(addZ - column.front()->GetPosition().z) >= offset) {
            AddPlatform(c, rowOffset);
        }
    }

    if (platformAddRow > Platform::ADD_ROW_MIN && platformAddRowTimer.IsElapsed()) {
        SetPlatformAddRow(platformAddRow - 1);

        platformAddRowTimer.SetElapsed(false);
    }

    if (platformAddEmptyRowsMax < Platform::ADD_EMPTY_ROWS_MAX &&
        platformAddEmptyRowsMaxTimer.IsElapsed())
    {
        SetPlatformAddEmptyRowsMax(platformAddEmptyRowsMax + 1);

        platformAddEmptyRowsMaxTimer.SetElapsed(false);
    }

    if (speed < Player::SPEED_MAX && speedIncrementTimer.IsElapsed()) {
        SetSpeed((min)(speed + Player::SPEED_INCREMENT, Player::SPEED_MAX));

        speedIncrementTimer.SetElapsed(false);
    }

    if (isSpeedupLocked && speedupLockTimer.IsElapsed()) {
        SetSpeedupLocked(false);

        player->SetColor(Player::COLOR);
        player->SetSpecial(false);

        speedupLockTimer.SetElapsed(false);
    }
}

PlayerCamera* Playground::GetCamera() const {
    return static_cast<PlayerCamera*>(Scene3D::GetCamera());
}

void Playground::SetCamera(std::unique_ptr<PlayerCamera> camera) {
    Scene3D::SetCamera(std::move(camera));
}

void Playground::AddObject(std::unique_ptr<PlaygroundObject> object) {
    Scene3D::AddObject(std::unique_ptr<Object3D> { object.release() });
}

std::unique_ptr<PlaygroundObject> Playground::RemoveObject(int id) {
    return std::unique_ptr<PlaygroundObject> {
        static_cast<PlaygroundObject*>(Scene3D::RemoveObject(id).release())
    };
}

std::unique_ptr<PlaygroundObject> Playground::RemoveObject(PlaygroundObject* object) {
    return std::unique_ptr<PlaygroundObject> {
        static_cast<PlaygroundObject*>(Scene3D::RemoveObject(object).release())
    };
}

Player* Playground::GetPlayer() const {
    return player;
}

void Playground::SetPlayer(std::unique_ptr<Player> player) {
    this->player = player.get();

    AddObject(std::unique_ptr<PlaygroundObject> { player.release() });
}

std::vector<std::list<Platform*>>& Playground::GetPlatforms() {
    return platforms;
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

void Playground::AddPlatform(int col, int rowOffset) {
    int i = col + Platform::COLUMN_RADIUS;
    auto&& column = platforms[i];

    float x = col * ceil(Platform::SIZE.x);
    float z = (Platform::REMOVE_ROW - rowOffset) * ceil(Platform::SIZE.z) + platformAddShifts[i];

    if (!column.empty()) {
        z = column.front()->GetPosition().z - rowOffset * ceil(Platform::SIZE.z);
    }

    auto platform = std::make_unique<Platform>();

    if (Random::Float() >= 0.95f) {
        platform = std::make_unique<Platform>(Platform::Type::FALL);

        if (lights.size() < 16) {
            auto light = std::make_unique<PointLight>();
            light->SetColor(platform->GetColor());
            platform->SetLight(light.get());
            AddLight(std::move(light));
        }
    }

    platform->SetPhysics(true);
    platform->SetPosition({ x, 0, z });
    platform->SetVelocity({ 0, 0, speedup * speed });
    platform->Object3D::SetAlpha(0);
    platforms[i].push_front(platform.get());

    if (Random::Float() >= 0.8f && platform->GetType() != Platform::Type::FALL) {
        // either obstacle (Type::SIMPLE) or pickup (!= Type::SIMPLE)
        auto addon = std::make_unique<Platform>(Platform::GetRandomType());

        if (addon->GetType() != Platform::Type::FALL) {
            addon->SetPhysics(true);
            addon->SetPosition({ x, 1, z });
            addon->SetVelocity({ 0, 0, speedup * speed });
            addon->Object3D::SetAlpha(0);
            platforms[i].push_front(addon.get());

            if (addon->GetType() != Platform::Type::SIMPLE) {
                addon->SetScale(0.5f);

                if (lights.size() < 16) {
                    auto light = std::make_unique<PointLight>();
                    light->SetColor(platform->GetColor());
                    addon->SetLight(light.get());
                    AddLight(std::move(light));
                }
            }
            else if (col != 0 && Random::Float() >= 0.6f) {
                for (int i = 2; i <= Random::Int(2, 4); ++i) {
                    auto addon2 = std::make_unique<Platform>();
                    addon2->SetPhysics(true);
                    addon2->SetPosition({ x, i, z });
                    addon2->SetVelocity({ 0, 0, speedup * speed });
                    addon2->Object3D::SetAlpha(0);
                    platforms[i].push_front(addon2.get());

                    AddObject(std::unique_ptr<PlaygroundObject> { addon2.release() });
                }
            }

            AddObject(std::unique_ptr<PlaygroundObject> { addon.release() });
        }
    }

    AddObject(std::unique_ptr<PlaygroundObject> { platform.release() });

    platformAddEmptyRows[i] = Random::Int(0, platformAddEmptyRowsMax);

    // make sure game is playable by making at least a column gap at most 1 row wide
    if (std::all_of(platformAddEmptyRows.begin(), platformAddEmptyRows.end(),
                    [](int emptyRows) { return emptyRows > 1; }))
    {
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
    if (speedupLocked) {
        speedupLockTimer.SetElapsed(false);
    }

    this->isSpeedupLocked = speedupLocked;
}

void Playground::ResetTimes() {
    platformAddRowTimer.SetElapsed(false);
    platformAddEmptyRowsMaxTimer.SetElapsed(false);

    speedIncrementTimer.SetElapsed(false);
    speedupLockTimer.SetElapsed(false);

    GetCamera()->ResetTimes();
}
