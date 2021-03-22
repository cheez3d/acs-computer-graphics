#pragma once

#include "Object2D.h"
#include "Space2D.h"

#include <Component/SimpleScene.h>

#include <chrono>
#include <vector>

namespace BowAndArrow {
    class Scene2D : public SimpleScene {
        friend class Object2D;

        Space2D world;
        Space2D worldBoundary;
        Space2D viewport;

        glm::mat3 model;
        glm::vec2 mouse;
        float gravity;
        std::chrono::time_point<std::chrono::steady_clock> time;

        bool isPaused;

        std::vector<std::unique_ptr<Object2D>> objects;
        std::vector<int> lastObjectsRemoved;

    protected:
        Scene2D() = default;

        Scene2D(Scene2D const& other) = delete;
        Scene2D& operator=(Scene2D const& other) = delete;

        Scene2D(Scene2D&& other) = default;
        Scene2D& operator=(Scene2D&& other) = default;

    public:
        virtual ~Scene2D() = default;

    protected:
        virtual void Init() override;

        //virtual void FrameStart() override;
        //virtual void Update(float dt) override;
        //virtual void FrameEnd() override;

        virtual void OnInputUpdate(float dt, int mods) override;

        virtual void OnKeyPress(int key, int mods) override;
        virtual void OnKeyRelease(int key, int mods) override;
        virtual void OnMouseMove(int mx, int my, int dx, int dy) override;
        virtual void OnMouseBtnPress(int mx, int my, int btn, int mods) override;
        virtual void OnMouseBtnRelease(int mx, int my, int btn, int mods) override;
        //virtual void OnMouseScroll(int mx, int my, int ox, int oy) override;
        virtual void OnWindowResize(int w, int h) override;

    private:
        void RenderMesh(std::string const& mesh, std::string const& shader, glm::mat3 model);

    public:
        bool IsKeyHeld(int key) const;
        bool IsMouseHeld(int btn) const;

    private:
        void InitMeshes();

    public:
        Space2D GetWorld() const;
        Space2D GetWorldBoundary() const; // TODO rename to Boundary

    private:
        void SetWorld(Space2D world);

    public:
        Space2D GetViewport() const;

    private:
        void SetViewport(Space2D viewport);

    public:
        glm::mat3 GetModel() const;

    private:
        void UpdateModel();

    public:
        glm::vec2 GetMouse() const;
        float GetGravity() const;
        std::chrono::time_point<std::chrono::steady_clock> GetTime() const;

        bool IsPaused() const;
        void SetPaused(bool paused);

        void AddObject(std::unique_ptr<Object2D> object);
        std::unique_ptr<Object2D> RemoveObject(int id);
        std::unique_ptr<Object2D> RemoveObject(Object2D* object);
    };
}
