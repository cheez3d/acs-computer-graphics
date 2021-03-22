#pragma once

#include "Activable.h"
#include "Controllable.h"

#include <include/glm.h>

namespace StylizedRunner {
    class Camera3D
        : public Activable
        , public Controllable
    {
        friend class Object3D;
        friend class Scene3D;

        glm::vec3 rotation;
        float radius;
        float fov;

        glm::vec3 right;
        glm::vec3 up;
        glm::vec3 forward;
        glm::vec3 position;
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;

        Object3D* object;
        Scene3D* scene;

    public:
        Camera3D();

    protected:
        Camera3D(Camera3D const& other);
        Camera3D& operator=(Camera3D const& other) = delete;

        Camera3D(Camera3D&& other) = default;
        Camera3D& operator=(Camera3D&& other) = default;

    public:
        virtual ~Camera3D() = default;

    protected:
        virtual void UpdateActive(float dt, float f, int mods) override;

        virtual void MouseMove(int dx, int dy) override;
        virtual void MouseScroll(int ox, int oy) override;

    public:
        virtual glm::vec3 GetRotation() const;
        virtual float GetRadius() const;
        virtual float GetFov() const;

        virtual void SetRotation(glm::vec3 rotation);
        void SetRotationX(float rotation);
        void SetRotationY(float rotation);
        void SetRotationZ(float rotation);
        virtual void SetRadius(float radius);
        virtual void SetFov(float fov);

    public:
        virtual glm::vec3 GetPosition() const;
        glm::mat4 GetModel() const;
        glm::mat4 GetView() const;
        glm::mat4 GetProjection() const;

    protected:
        virtual void SetPosition(glm::vec3 position);

    private:
        void UpdateModel();
        void UpdateView();
        void UpdateProjection();

    public:
        Object3D* GetObject() const;
        Scene3D* GetScene() const;

    protected:
        void SetObject(Object3D* object);
        virtual void SetScene(Scene3D* scene);
    };
}
