#pragma once

#include "Cloneable.h"
#include "Transform3D.h"

#include <include/glm.h>

#include <memory>

namespace StylizedRunner {
    template<class T>
    class Collidable {
    protected:
        class Bounding;
        class Behavior;

    private:
        bool hasCollisions;
        bool isDynamic;
        std::unique_ptr<Bounding> bounding;

    protected:
        Collidable() {
            SetCollisions(false);
            SetDynamic(false);
            SetBounding(std::make_unique<Bounding::Sphere>());
        }

        Collidable(Collidable const& other) {
            SetCollisions(other.hasCollisions);
            SetDynamic(other.isDynamic);
            SetBounding(other.bounding->Clone());
        }

        Collidable& operator=(Collidable const& other) = delete;

        Collidable(Collidable&& other) = default;
        Collidable& operator=(Collidable&& other) = default;

    public:
        virtual ~Collidable() = default;

        bool HasCollisions() const {
            return hasCollisions;
        }

        bool IsDynamic() const {
            return isDynamic;
        }

        void SetCollisions(bool collisions) {
            this->hasCollisions = collisions;
        }

        void SetDynamic(bool dynamic) {
            this->isDynamic = dynamic;
        }

    protected:
        virtual void UpdateCollisions(T* other) = 0;

    public:
        virtual Bounding* GetBounding() const {
            return bounding.get();
        }

    protected:
        virtual void SetBounding(std::unique_ptr<Bounding> bounding) { // TODO make pure virtual
            this->bounding = std::move(bounding);
        }

    public:
        virtual bool CollidesWith(T const* other) const = 0;

    protected:
        virtual void CollidedWith(T* other) = 0;
    };

    template<class T>
    class Collidable<T>::Bounding : public Cloneable<Collidable<T>::Bounding> {
        friend class Object3D;

    public:
        class Box;
        class Sphere;

    private:
        glm::vec3 position;
        glm::vec3 offset;
        glm::vec3 size;

        T const* parent;

    public:
        Bounding() {
            SetPosition({ 0, 0, 0 });
            SetOffset({ 0, 0, 0 });
            SetSize({ 0, 0, 0 });

            SetParent(nullptr);
        }

        Bounding(Bounding const& other) {
            SetPosition(other.position);
            SetOffset(other.offset);
            SetSize(other.size);

            SetParent(nullptr);
        }

        Bounding& operator=(Bounding const& other) = delete;

        Bounding(Bounding&& other) = default;
        Bounding& operator=(Bounding&& other) = default;

        virtual ~Bounding() = default;

        glm::mat4 GetModel() const {
            using namespace Transform3D;

            return Translate(parent->GetPosition())
                 * Scale(parent->GetScale());
        }

        glm::vec3 GetPosition() const {
            return position;
        }

    protected:
        void SetPosition(glm::vec3 position) {
            this->position = position;
        }

    public:
        glm::vec3 GetOffset() const {
            return offset;
        }

    protected:
        void SetOffset(glm::vec3 offset) {
            this->offset = offset;
        }

    public:
        glm::vec3 GetSize() const {
            return size;
        }

    protected:
        void SetSize(glm::vec3 size) {
            this->size = size;
        }

    public:
        T const* GetParent() const {
            return parent;
        }

    private:
        void SetParent(T const* parent) {
            this->parent = parent;
        }

    public:
        virtual bool CollidesBox(Box const* box) const = 0;
        virtual bool CollidesSphere(Sphere const* sphere) const = 0;
    };

    template<class T>
    class Collidable<T>::Bounding::Box : public Collidable<T>::Bounding {
        friend class Platform;

    protected:
        virtual Box* CloneImplementation() const override {
            return new Box(*this);
        };

    public:
        glm::vec3 GetMin() const {
            return GetPosition() - GetSize() / 2.0f;
        }

        glm::vec3 GetMax() const {
            return GetMin() + GetSize();
        }

        virtual bool CollidesBox(Box const* box) const override {
            using namespace Transform3D;

            vec3 min = GetModel() * vec4 { GetMin(), 1 };
            vec3 max = GetModel() * vec4 { GetMax(), 1 };

            vec3 boxMin = box->GetModel() * vec4 { box->GetMin(), 1 };
            vec3 boxMax = box->GetModel() * vec4 { box->GetMax(), 1 };

            return all(lessThanEqual(min - boxMax, vec3 { 0 })) &&
                   all(greaterThanEqual(max - boxMin, vec3 { 0 }));
        }

        virtual bool CollidesSphere(Sphere const* sphere) const override {
            return sphere->CollidesBox(this);
        }
    };

    template<class T>
    class Collidable<T>::Bounding::Sphere : public Collidable<T>::Bounding {
        friend class Player;

    protected:
        virtual Sphere* CloneImplementation() const override {
            return new Sphere(*this);
        };

    public:
        float GetRadius() const {
            return ((GetSize().x + GetSize().y + GetSize().z) / 3) / 2;
        }

        virtual bool CollidesBox(Box const* box) const override {
            using namespace glm;

            vec3 position = GetModel() * vec4 { GetPosition(), 1 };
            float radius = (GetModel() * vec4 { vec3 { GetRadius() }, 0 }).x;

            vec3 boxMin = box->GetModel() * vec4 { box->GetMin(), 1 };
            vec3 boxMax = box->GetModel() * vec4 { box->GetMax(), 1 };

            vec3 p = clamp(position, boxMin, boxMax);

            float d2 = distance2(p, position);
            float r2 = pow(radius, 2);

            return d2 <= r2;
        }

        virtual bool CollidesSphere(Sphere const* sphere) const override {
            using namespace glm;

            vec3 position = GetModel() * vec4 { GetPosition(), 1 };
            float radius = (GetModel() * vec4 { vec3 { GetRadius() }, 0 }).x;

            vec3 spherePosition = GetModel() * vec4 { GetPosition(), 1 };
            float sphereRadius = (sphere->GetModel() * vec4 { vec3 { sphere->GetRadius() }, 0 }).x;

            float d2 = glm::distance2(position, spherePosition);
            float r2 = pow(radius + sphereRadius, 2);

            return d2 <= r2;
        }
    };

    template<>
    class Collidable<Object3D>::Behavior {
        friend class FuelGauge;
        friend class Platform;
        friend class Player;

    protected:
        virtual void CollidedFuelGauge(FuelGauge* fuelIndicator) {};
        virtual void CollidedPlatform(Platform* arrow) {};
        virtual void CollidedPlayer(Player* balloon) {};
    };
}
