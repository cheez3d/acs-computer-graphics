#pragma once

#include "Activable.h"
#include "Tema3/Cloneable.h"
#include "Collidable.h"
#include "Controllable.h"

#include <Core/Engine.h>

#include <memory>

namespace Skyroads {
    class Object3D
        : public Activable
        , public Cloneable<Object3D>
        , public Collidable<Object3D>
        , public Collidable<Object3D>::Behavior
        , public Controllable
    {
        friend class Camera3D;
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
        std::string shader;

        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 rotation;
        glm::vec3 scale;
        glm::vec3 color;
        float alpha;

        glm::mat4 model;
        bool hasPhysics;
        bool isVisible;

        Object3D* parent;

    protected:
        std::vector<std::unique_ptr<Object3D>> attachments;

        Scene3D* scene;

    protected:
        Object3D();

        Object3D(Object3D const& other);
        Object3D& operator=(Object3D const& other) = delete;

        Object3D(Object3D&& other) = default;
        Object3D& operator=(Object3D&& other) = default;

    public:
        virtual ~Object3D() = default;

    protected:
        virtual void SetBounding(std::unique_ptr<Bounding> bounding) override;

    public:
        std::string GetShader() const;
        void SetShader(std::string shader);

        glm::vec3 GetPosition() const;
        void SetPosition(glm::vec3 position);

        glm::vec3 GetVelocity() const;
        void SetVelocity(glm::vec3 velocity);

        glm::vec3 GetRotation() const;
        void SetRotation(glm::vec3 rotation);
        void SetRotationX(float rotation);
        void SetRotationY(float rotation);
        void SetRotationZ(float rotation);

        glm::vec3 GetScale() const;
        void SetScale(glm::vec3 scale);
        void SetScale(float scale);

        virtual glm::vec3 GetColor() const;
        virtual void SetColor(glm::vec3 color);

        virtual float GetAlpha() const;
        virtual void SetAlpha(float alpha);

        glm::mat4 GetModel() const;

    private:
        void UpdateModel();

    protected:
        virtual void UpdateActive(float f, int mods) override;

    public:
        bool HasPhysics() const;
        void SetPhysics(bool physics);

    protected:
        virtual void UpdatePhysics(float f, int mods);

    public:
        bool IsVisible() const;
        void SetVisible(bool visible);

    protected:
        virtual void PrepareShader(Shader* shader, glm::mat4 model, int component) const;

    private:
        void Render(float f, int mods);
        
    public:
        Object3D* Object3D::GetParent() const;
        void AddAttachment(int id, std::unique_ptr<Object3D> attachment);
        std::unique_ptr<Object3D> CloneAttachment(int id) const;
        std::unique_ptr<Object3D> RemoveAttachment(int id);

        virtual Scene3D* GetScene() const;

    private:
        void SetScene(Scene3D* scene);


    protected:
        virtual void UpdateCollisions(Object3D* other) override;
    };
}
