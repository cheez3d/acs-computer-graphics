#pragma once

#include "FuelGauge.h"
#include "PlaygroundObject.h"
#include "Timer.h"

#include <memory>

class Shader;

namespace StylizedRunner {
    class PlayerCamera;
    class Playground;

    class Player final : public PlaygroundObject {
        enum Component : decltype(components)::size_type {
            SPHERE,
            COMPONENT_COUNT,
        };

    public:
        static constexpr float DIAMETER = 0.5f;

        static constexpr int COLUMN_START = 0;
        static glm::vec3 const POSITION_START;

        static constexpr glm::vec3 COLOR { 1, 74.0f / 255, 249.0f / 255 };

        static constexpr int FALL_FIRST_PERSON_Y = -32;
        static constexpr int FALL_RESET_Y = -64;

        static constexpr float SPEED_START = 1;
        static constexpr float SPEED_MAX = 8;
        static constexpr float SPEED_INCREMENT = 0.1f;
        static constexpr float SPEEDUP = 2;
        static constexpr float FORCE_SPEEDUP = 2;

    private:
        glm::vec3 color;
        float alpha;

        int column;
        bool isSpecial;
        std::unique_ptr<FuelGauge> fuelGauge;

        Timer<float> jumpTimer;
        Timer<float> fuelTimer;

    public:
        Player();

        Player(Player const& other);
        Player& operator=(Player const& other) = delete; // TODO copy and swap

        Player(Player&& other) = default;
        Player& operator=(Player&& other) = default;

    protected:
        virtual void UpdateActive(float dt, float f, int mods) override;

        virtual Player* CloneImplementation() const override;

    public:
        virtual Bounding::Sphere* GetBounding() const override;

        virtual bool CollidesWith(Object3D const* other) const override;

    protected:
        virtual void CollidedWith(Object3D* other) override;

        virtual void CollidedPlatform(Platform* platform) override;

        virtual void KeyChange(int key, int mods, bool press) override;

        virtual void UpdatePhysics(float dt, float f, int mods) override;

        virtual void Render(float dt, float f, int mods) override;

    public:
        virtual glm::vec3 GetColor() const override;
        virtual float GetAlpha() const override;

        virtual void SetColor(glm::vec3 color) override;
        virtual void SetAlpha(float alpha) override;

    private:
        virtual void UpdateShader(Shader* shader, glm::mat4 model, int component) const override;

        using Object3D::GetCamera;
        using Object3D::GetScene;

        using Object3D::SetCamera;
        virtual void SetScene(Scene3D* scene) override;

    public:
        PlayerCamera* GetCamera() const;
        Playground* GetScene() const;

        void SetCamera(PlayerCamera* camera);
        void SetScene(Playground* scene);

    public:
        int GetColumn() const;
        bool IsSpecial() const;
        FuelGauge* GetFuelGauge() const;

        void SetColumn(int column);
        void SetSpecial(bool special);
        void SetFuelGauge(std::unique_ptr<FuelGauge> fuelGauge);

        void KillCamera();
        void Kill();
        void Reset();
    };
}
