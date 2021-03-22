#pragma once

#include "Activable.h"
#include "Camera3D.h"
#include "Controllable.h"
#include "Light3D.h"
#include "Object3D.h"
#include "Physics.h"
#include "Renderable.h"
#include "Watch.h"

#include <Component/SimpleScene.h>
#undef GetObject // fixes conflict with <Windows.h> `GetObject` macro

#include <map>
#include <vector>

class Texture2D;

namespace StylizedRunner {
    class Scene3D
        : public SimpleScene
        , public Activable
//      , public Collidable<?>
        , public Controllable
        , public Physics
        , public Renderable
    {
        friend class Camera3D;
        friend class Light3D;
        friend class Object3D;

    public: // TEMP for player stop rotation on obstacle hit
        static inline float dt = 0;

    private:
        static constexpr int FPS = 60;

        std::unordered_map<std::string, Texture2D*> textures;

        std::unique_ptr<Watch<float>> watch;
        glm::vec2 mouse;
        glm::vec3 gravity;

        bool isPaused;

        std::unique_ptr<Camera3D> camera;

        std::vector<std::unique_ptr<Object3D>> objects;
        std::multimap<float, Object3D*, std::greater<float>> objectsBlending;
        std::vector<Object3D*> objectsGui;
        std::vector<int> objectIdsRemoved;

    protected: // temp for lights.size() in Playground TODO
        std::vector<std::unique_ptr<Light3D>> lights;

    protected:
        Scene3D();

        Scene3D(Scene3D const& other) = delete;
        Scene3D& operator=(Scene3D const& other) = delete;

        Scene3D(Scene3D&& other) = default;
        Scene3D& operator=(Scene3D&& other) = default;

    public:
        virtual ~Scene3D() = default;

    protected:
        virtual void Init() override;

    private:
        void InitMeshes();
        std::unordered_map<std::string, Mesh*>& GetMeshes();

        void InitShaders();
        std::unordered_map<std::string, Shader*>& GetShaders();

        void InitTextures();
        std::unordered_map<std::string, Texture2D*>& GetTextures();

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

    protected:
        virtual void UpdateActive(float dt, float f, int mods) override;

    private:
        void UpdateCollisions(float dt, float f, int mods); // TODO inherit `Collidable`

        virtual void KeyChange(int key, int mods, bool press) override;
        virtual void MouseChange(int btn, int mods, bool press) override;
        virtual void MouseMove(int dx, int dy) override;
        virtual void MouseScroll(int ox, int oy) override;

        virtual void UpdatePhysics(float dt, float f, int mods) override;

        virtual void Render(float dt, float f, int mods) override;

    public:
        bool IsKeyHeld(int key) const;
        bool IsMouseHeld(int btn) const;

        Watch<float>* GetWatch() const;
        glm::vec2 GetMouse() const;
        glm::vec3 GetGravity() const;

    private:
        void SetWatch(std::unique_ptr<Watch<float>> watch);

    public:
        void SetGravity(glm::vec3 gravity);

    public:
        Camera3D* GetCamera() const;

    protected:
        void SetCamera(std::unique_ptr<Camera3D> camera);

        void AddObject(std::unique_ptr<Object3D> object);
        std::unique_ptr<Object3D> RemoveObject(int id);
        std::unique_ptr<Object3D> RemoveObject(Object3D* object);

    private:
        void FixObjectId(int& id);

    protected:
        void AddLight(std::unique_ptr<Light3D> light);
        std::unique_ptr<Light3D> RemoveLight(int id);
        std::unique_ptr<Light3D> RemoveLight(Light3D* light);

    public:
        bool IsPaused() const;
        void SetPaused(bool paused);
    };
}
