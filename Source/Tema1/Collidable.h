#pragma once

#include <include/glm.h>

#include <utility>

namespace BowAndArrow {
    template<class T>
    class Collidable {
    protected:
        class Behavior;

        enum BoundsProperty : unsigned { // TODO rename Bounds to Bounding
            POSITION, RADIUS,
        };

    private:
        bool hasCollisions;

        std::pair<glm::vec2, float> bounds;

    protected:
        Collidable() {
            SetCollisions(true);

            bounds = { { 0, 0 }, 0 };
        }

    public:
        virtual ~Collidable() = default;

        bool HasCollisisons() const {
            return hasCollisions;
        }

        void SetCollisions(bool collisions) {
            this->hasCollisions = collisions;
        }

    protected:
        virtual void UpdateCollisions(T* other) = 0;

    public:
        virtual std::pair<glm::vec2, float> GetBounds() const {
            return bounds;
        }

    protected:
        void SetBounds(std::pair<glm::vec2, float> bounds) {
            this->bounds = bounds;
        }

        void SetBoundsPosition(glm::vec2 position) {
            std::get<POSITION>(bounds) = position;
        }

        void SetBoundsRadius(float radius) {
            std::get<RADIUS>(bounds) = radius;
        }

    public:
        virtual bool CollidesWith(T const* other) const = 0;

    protected:
        virtual void CollidedWith(T* other) = 0;
    };

    class Object2D;

    template<>
    class Collidable<Object2D>::Behavior {
        friend class Arrow;
        friend class Balloon;
        friend class Bow;
        friend class Heart;
        friend class Shuriken;

    protected:
        Behavior() = default;

    public:
        virtual ~Behavior() = default;

    protected:
        virtual void CollidedArrow(Arrow* arrow) {};
        virtual void CollidedBalloon(Balloon* balloon) {};
        virtual void CollidedBow(Bow* bow) {};
        virtual void CollidedHeart(Heart* heart) {};
        virtual void CollidedShuriken(Shuriken* shuriken) {};
    };
}
