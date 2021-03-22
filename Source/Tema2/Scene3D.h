#pragma once

#include "Camera3D.h"
#include "Object3D.h"

#include <Component/SimpleScene.h>

#include <chrono>
#include <vector>

namespace Skyroads {
    class Scene3D : public SimpleScene {
        friend class Camera3D;
        friend class Object3D;

        std::unique_ptr<Camera3D> camera;
        glm::vec3 light;

        glm::mat4 projection;
        glm::vec2 mouse;
        float gravity;
        std::chrono::time_point<std::chrono::steady_clock> time;

        bool isPaused;

        std::vector<std::unique_ptr<Object3D>> objects;
        std::vector<int> lastObjectsRemoved;

    protected:
        Scene3D() = default;

        Scene3D(Scene3D const& other) = delete;
        Scene3D& operator=(Scene3D const& other) = delete;

        Scene3D(Scene3D&& other) = default;
        Scene3D& operator=(Scene3D&& other) = default;

    public:
        virtual ~Scene3D() = default;

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
        virtual void OnMouseScroll(int mx, int my, int ox, int oy) override;
        virtual void OnWindowResize(int w, int h) override;

    public:
        bool IsKeyHeld(int key) const;
        bool IsMouseHeld(int btn) const;

    private:
        void InitMeshes();
        std::unordered_map<std::string, Mesh*>& GetMeshes();

        void InitShaders();
        std::unordered_map<std::string, Shader*>& GetShaders();

    public:
        Camera3D* GetCamera() const;

    protected:
        void SetCamera(std::unique_ptr<Camera3D> camera);

    public:
        virtual glm::vec3 GetLight() const;

    protected:
        virtual void SetLight(glm::vec3 light);

    public:
        glm::mat4 GetProjection() const;

    private:
        void UpdateProjection();

    public:
        float GetGravity() const;
        std::chrono::time_point<std::chrono::steady_clock> GetTime() const;

        bool IsPaused() const;
        void SetPaused(bool paused);

        void AddObject(std::unique_ptr<Object3D> object);
        std::unique_ptr<Object3D> RemoveObject(int id);
        std::unique_ptr<Object3D> RemoveObject(Object3D* object);
    };
}
