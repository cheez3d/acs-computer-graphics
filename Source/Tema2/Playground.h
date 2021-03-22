#pragma once

#include "Scene3D.h"

#include <chrono>
#include <list>
#include <vector>

namespace Skyroads {
    class Playground final : public Scene3D  {
    public:
        static inline float const LIGHT_Y = 5;

    private:
        glm::vec3 light;

        Player* player;
        FuelIndicator* fuelIndicator;
        std::chrono::time_point<std::chrono::steady_clock> lastFuelIndicatorLevelDecrement;

        std::vector<std::list<Platform*>> platforms;
        int platformAddRow;
        std::chrono::time_point<std::chrono::steady_clock> lastPlatformAddRowIncrement;
        std::vector<float> platformAddShifts;
        std::vector<int> platformAddEmptyRows;
        int platformAddEmptyRowsMax;
        std::chrono::time_point<std::chrono::steady_clock> lastPlatformAddEmptyRowsMaxIncrement;

        float speed;
        float speedup;
        std::chrono::time_point<std::chrono::steady_clock> lastSpeedIncrement;
        bool isSpeedupLocked;
        std::chrono::time_point<std::chrono::steady_clock> lastSpeedupLock;

    protected:
        virtual void Init() override;

        virtual void OnInputUpdate(float dt, int mods) override;

    public:
        virtual glm::vec3 GetLight() const override;

    protected:
        virtual void SetLight(glm::vec3 light) override;

    public:
        Player* GetPlayer() const;
        FuelIndicator* GetFuelIndicator() const;

        std::vector<std::list<Platform*>>& GetPlatforms();
        void AddInitialPlatforms();
        void RemovePlatforms();
        void SetPlatformAddRow(int platformAddRow);
        void SetPlatformAddEmptyRowsMax(int platformAddEmptyRowsMax);
        void AddPlatform(int column, int row);

        float GetSpeed() const;
        void SetSpeed(float speed);

        float GetSpeedup() const;
        void SetSpeedup(float speedup);

        bool IsSpeedupLocked() const;
        void SetSpeedupLocked(bool speedupLocked);

        void ResetTimes();
    };
}
