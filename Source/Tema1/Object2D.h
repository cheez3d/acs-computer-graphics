#pragma once

#include "Tema3/Cloneable.h"
#include "Collidable.h"

#include <Core/Engine.h>

#include <memory>
#include <vector>

namespace BowAndArrow {
    class Object2D
        : public Cloneable<Object2D>
        , public Collidable<Object2D>
        , public Collidable<Object2D>::Behavior
    {
        friend class Scene2D;

    protected:
        enum ComponentProperty : unsigned {
            NAME, OBJECT_MODEL, USER_MODEL,
        };

        std::vector<std::tuple<std::string, glm::mat3, glm::mat3>> components;

    private:
        glm::vec2 position;
        glm::vec2 offset;
        glm::vec2 velocity;
        float rotation;
        glm::vec2 scale;

        glm::mat3 model;
        bool isAlive;
        bool hasPhysics;
        bool isVisible;

        Object2D* parent;

    protected:
        std::vector<std::unique_ptr<Object2D>> attachments;

    private:
        Scene2D* scene;

    protected:
        Object2D();

        Object2D(Object2D const& other);
        Object2D& operator=(Object2D const& other) = delete;

        Object2D(Object2D&& other) = default;
        Object2D& operator=(Object2D&& other) = default;

    public:
        virtual ~Object2D() = default;

        glm::vec2 GetPosition() const;
        void SetPosition(glm::vec2 position);

        glm::vec2 GetOffset() const;
        void SetOffset(glm::vec2 offset);

        glm::vec2 GetVelocity() const;
        void SetVelocity(glm::vec2 velocity);

        glm::vec2 GetScale() const;
        void SetScale(glm::vec2 scale);
        void SetScale(float scale);

        float GetRotation() const;
        void SetRotation(float rotation);

        glm::mat3 GetModel() const;

    private:
        void UpdateModel();

    public:
        bool IsAlive() const;
        void SetAlive(bool alive);

    protected:
        virtual void UpdateAlive(float f, int mods);

    public:
        bool HasPhysics() const;
        void SetPhysics(bool physics);

    protected:
        virtual void UpdatePhysics(float f, int mods);

        virtual void KeyChange(int key, int mods, bool press) {}
        virtual void MouseChange(int btn, int mods, bool press) {}

    public:
        bool IsVisible() const;
        void SetVisible(bool visible);

    private:
        void Update(float f, int mods);

    public:
        Object2D* Object2D::GetParent() const;
        void AddAttachment(int id, std::unique_ptr<Object2D> attachment);
        std::unique_ptr<Object2D> CloneAttachment(int id) const;
        std::unique_ptr<Object2D> RemoveAttachment(int id);

        virtual Scene2D* GetScene() const;

    private:
        void SetScene(Scene2D* scene);

    protected:
        virtual void UpdateCollisions(Object2D* other) override;

    public:
        virtual std::pair<glm::vec2, float> GetBounds() const override;

        virtual bool CollidesWith(Object2D const* other) const override;
    };
}
