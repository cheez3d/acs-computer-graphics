#pragma once

#include "Player.h"
#include "PlaygroundObject.h"
#include "Scene3D.h"
#include "Timer.h"

#include <irrKlang/irrKlang.h>

#include <list>
#include <vector>

namespace StylizedRunner {
    class Platform;

    class Playground final : public Scene3D  {
    public: // TODO
        std::unique_ptr<irrklang::ISoundEngine> soundEngine;
    private:
        Timer<float> musicTimer;

        Player* player;

        std::vector<std::list<Platform*>> platforms;
        int platformAddRow;
        std::vector<float> platformAddShifts;
        std::vector<int> platformAddEmptyRows;
        int platformAddEmptyRowsMax;

        float speed;
        float speedup;
        bool isSpeedupLocked;

        Timer<float> platformAddRowTimer;
        Timer<float> platformAddEmptyRowsMaxTimer;

        Timer<float> speedIncrementTimer;
        Timer<float> speedupLockTimer;

    public:
        Playground();

    private:
        virtual void Init() override;

        virtual void UpdateActive(float dt, float f, int mods) override;

    private:
        using Scene3D::GetCamera;

        using Scene3D::SetCamera;

    public:
        PlayerCamera* GetCamera() const;

    private:
        void SetCamera(std::unique_ptr<PlayerCamera> camera);

        using Scene3D::AddObject;
        using Scene3D::RemoveObject;

    public:
        void AddObject(std::unique_ptr<PlaygroundObject> object);
        std::unique_ptr<PlaygroundObject> RemoveObject(int id);
        std::unique_ptr<PlaygroundObject> RemoveObject(PlaygroundObject* object);

        Player* GetPlayer() const;
        std::vector<std::list<Platform*>>& GetPlatforms();
        float GetSpeed() const;
        float GetSpeedup() const;
        bool IsSpeedupLocked() const;

    private:
        void SetPlayer(std::unique_ptr<Player> player);

    public:
        void SetSpeed(float speed);
        void SetSpeedup(float speedup);
        void SetSpeedupLocked(bool speedupLocked);

        void RemovePlatforms();
        void SetPlatformAddRow(int platformAddRow);
        void SetPlatformAddEmptyRowsMax(int platformAddEmptyRowsMax);
        void AddPlatform(int col, int row);

        void ResetTimes();
    };
}
