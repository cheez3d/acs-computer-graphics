#pragma once

#include "Scene2D.h"

#include <chrono>
// TODO change `Alive` to `Active` -> add Activable interface, add Controllable interface
// TODO split Update method in parts call each separately in Scene2D OnInputUpdate
// TODO move RenderMesh from Scene2D to Object2D (call meshes[mesh]->Render())
// TODO move WorldToAttachment and AttachmentToWorld in Object3D as static methods
// TODO make bow elevation Lerp like in PlayerCamera with inheritance (maybe)
// TODO do the same with the glClearColor for Scene2D
// TODO make certain classes final
// TODO add corresponding `SetParent` method to `Object2D`
// TODO add `SetGravity` method
// TODO port `Watch` system to here
// TODO port collisions system to here
namespace BowAndArrow {
    class Playground : public Scene2D {
    public:
        static inline const glm::vec3 COLOR_NORMAL { 0, 97.f / 255, 27.f / 255 };
        static inline const glm::vec3 COLOR_DAMAGE { 122.0f / 255, 12.0f / 255, 0 };

        static inline float const BOW_INVINCIBILITY_DELAY = 1;

        static inline float const BALLOON_ADD_DELAY = 0.5f;

        static inline float const SHURIKEN_ADD_DELAY = 3;

    private:
        glm::vec3 color;

        Bow* bow;
        std::vector<Heart*> hearts;

        std::chrono::time_point<std::chrono::steady_clock> lastBowInvincibility;

        std::chrono::time_point<std::chrono::steady_clock> lastBalloonAdd;

        std::chrono::time_point<std::chrono::steady_clock> lastShurikenAdd;

        unsigned score;

    protected:
        virtual void Init() override;

        virtual void OnInputUpdate(float dt, int mods) override;

    public:
        Bow* GetBow() const;

        void ShowDamage();
        void TakeHeart();

        unsigned GetScore() const;
        void ShowScore() const;
        void AddScore(int score);
    };
}
