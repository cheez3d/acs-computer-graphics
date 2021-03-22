#pragma once

#include "Activable.h"

#include <include/glm.h>

namespace StylizedRunner {
    class Object3D;
    class Scene3D;

    class Light3D : public Activable {
        friend class Object3D;
        friend class Scene3D;

        glm::vec3 direction;
        float cutoff;
        glm::vec3 color;
        float alpha;

        glm::vec3 position;
        glm::mat4 model;

        Object3D* object;
        Scene3D* scene;

    protected:
        Light3D();

        Light3D(Light3D const& other);
        Light3D& operator=(Light3D const& other) = delete;

        Light3D(Light3D&& other) = default;
        Light3D& operator=(Light3D&& other) = default;

    public:
        virtual ~Light3D() = default;

    protected:
        virtual void UpdateActive(float dt, float f, int mods) override;

    public:
        glm::vec3 GetDirection() const;
        float GetCutoff() const;
        glm::vec3 GetColor() const;
        float GetAlpha() const;

        void SetDirection(glm::vec3 direction);
        void SetCutoff(float cutoff);
        void SetColor(glm::vec3 color);
        void SetAlpha(float alpha);

        glm::vec3 GetPosition() const;
        glm::mat4 GetModel() const;

    protected:
        void SetPosition(glm::vec3 position);

    private:
        void UpdateModel();

    public:
        Object3D* GetObject() const;
        Scene3D* GetScene() const;

    protected:
        void SetObject(Object3D* object);
        virtual void SetScene(Scene3D* scene);
    };
}
