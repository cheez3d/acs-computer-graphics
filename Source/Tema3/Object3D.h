#pragma once

#include "Activable.h"
#include "Cloneable.h"
#include "Collidable.h"
#include "Controllable.h"
#include "Gui.h"
#include "Physics.h"
#include "Renderable.h"

#include <include/glm.h>

#include <memory>
#include <string>
#include <vector>

class Shader;

namespace StylizedRunner {
    class Object3D
        : public Activable
        , public Cloneable<Object3D>
        , public Collidable<Object3D>
        , public Collidable<Object3D>::Behavior
        , public Controllable
        , public Gui
        , public Physics
        , public Renderable
    {
        friend class Camera3D;
        friend class Light3D;
        friend class Scene3D;

    protected:
        enum ComponentProperty : unsigned {
            NAME, OBJECT_MODEL, USER_MODEL,
        };

    public:
        static void Object3D::AttachmentToParent(Object3D const* parent, Object3D* attachment);
        static void Object3D::AttachmentToWorld(Object3D const* parent, Object3D* attachment);

    protected:
        std::vector<std::tuple<std::string, glm::mat4, glm::mat4>> components;

    private:
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 rotation;
        glm::vec3 scale;
        glm::vec3 color;
        float alpha;

        glm::mat4 model;

        Camera3D* camera;
        Light3D* light;
        Scene3D* scene;

    protected:
        std::vector<std::unique_ptr<Object3D>> attachments;

    private:
        Object3D* parent;

    protected:
        Object3D();

        Object3D(Object3D const& other);
        Object3D& operator=(Object3D const& other) = delete;

        Object3D(Object3D&& other) = default;
        Object3D& operator=(Object3D&& other) = default;

    public:
        virtual ~Object3D() = default;

    protected:
        virtual void UpdateActive(float dt, float f, int mods) override;

    private:
        virtual void UpdateCollisions(Object3D* other) override;

    protected:
        virtual void SetBounding(std::unique_ptr<Bounding> bounding) override;

        virtual void UpdatePhysics(float dt, float f, int mods) override;

        virtual void UpdateShader(Shader* shader, glm::mat4 model, int componentId) const override;
        virtual void Render(float dt, float f, int mods) override;

    public:
        glm::vec3 GetPosition() const;
        glm::vec3 GetVelocity() const;
        glm::vec3 GetRotation() const;
        glm::vec3 GetScale() const;
        virtual glm::vec3 GetColor() const;
        virtual float GetAlpha() const;

        void SetPosition(glm::vec3 position);
        void SetVelocity(glm::vec3 velocity);
        void SetRotation(glm::vec3 rotation);
        void SetRotationX(float rotation);
        void SetRotationY(float rotation);
        void SetRotationZ(float rotation);
        void SetScale(glm::vec3 scale);
        void SetScale(float scale);
        virtual void SetColor(glm::vec3 color);
        virtual void SetAlpha(float alpha);

        glm::mat4 GetModel() const;

    private:
        void UpdateModel();

    public:
        Camera3D* GetCamera() const;
        Light3D* GetLight() const;
        Scene3D* GetScene() const;

    protected:
        void SetCamera(Camera3D* camera);

    public:
        void SetLight(Light3D* light);

    protected:
        virtual void SetScene(Scene3D* scene);

    public:
        void AddAttachment(int id, std::unique_ptr<Object3D> attachment);
        std::unique_ptr<Object3D> CloneAttachment(int id) const;
        std::unique_ptr<Object3D> RemoveAttachment(int id);

    public:
        Object3D* GetParent() const;

    private:
        void SetParent(Object3D* parent);
    };
}
