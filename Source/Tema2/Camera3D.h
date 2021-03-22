#pragma once

#include "Activable.h"
#include "Controllable.h"

#include <include/glm.h>

namespace Skyroads {
    class Camera3D
        : public Activable
        , public Controllable
    {
        friend class Object3D;
        friend class Scene3D;

    public:
        static inline float const DEFAULT_RADIUS = 1;
        static inline float const DEFAULT_FOV = 60;

    private:
        Object3D* target;
        glm::vec3 rotation;
        float radius;
        float fov;

        glm::vec3 right;
        glm::vec3 up;
        glm::vec3 forward;
        glm::vec3 position;

        Scene3D* scene;

    public:
        Camera3D();

        Camera3D(Camera3D const& other) = delete;
        Camera3D& operator=(Camera3D const& other) = delete;

        Camera3D(Camera3D&& other) = default;
        Camera3D& operator=(Camera3D&& other) = default;

        virtual ~Camera3D() = default;

    protected:
        virtual void MouseMove(int mx, int my, int dx, int dy) override;
        virtual void MouseScroll(int mx, int my, int ox, int oy) override;

    public:
        virtual Object3D* GetTarget() const;
        void SetTarget(Object3D* target);

        virtual glm::vec3 GetRotation() const;
        virtual void SetRotation(glm::vec3 rotation);
        void SetRotationX(float rotation);
        void SetRotationY(float rotation);
        void SetRotationZ(float rotation);

        virtual float GetRadius() const;
        virtual void SetRadius(float radius);

        virtual float GetFov() const;
        virtual void SetFov(float fov);

        virtual glm::vec3 GetPosition() const;

    protected:
        virtual void SetPosition(glm::vec3 position);

    public:
        glm::mat4 GetView() const;

        glm::mat4 GetModel() const;

    private:
        void UpdateModel();

    public:
        virtual Scene3D* GetScene() const;
        void SetScene(Scene3D* scene);
    };
}
