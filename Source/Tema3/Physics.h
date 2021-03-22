#pragma once

class Physics {
    bool hasPhysics;
    float drag;

protected:
    Physics() {
        SetPhysics(false);
        SetDrag(0);
    }

    Physics(Physics const& other) {
        SetPhysics(other.hasPhysics);
        SetDrag(other.drag);
    }

    Physics& operator=(Physics const& other) = delete;

    Physics(Physics&& other) = default;
    Physics& operator=(Physics&& other) = default;

public:
    virtual ~Physics() = default;

    bool HasPhysics() const {
        return hasPhysics;
    }

    float GetDrag() const {
        return drag;
    }

    void SetPhysics(bool physics) {
        this->hasPhysics = physics;
    }

    void SetDrag(float drag) {
        this->drag = drag;
    }

protected:
    virtual void UpdatePhysics(float dt, float f, int mods) {}
};
