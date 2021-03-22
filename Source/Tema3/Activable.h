#pragma once

class Activable {
    bool isActive;

protected:
    Activable() {
        SetActive(true);
    }

    Activable(Activable const& other) {
        SetActive(other.isActive);
    }

    Activable& operator=(Activable const& other) = delete;

    Activable(Activable&& other) = default;
    Activable& operator=(Activable&& other) = default;

public:
    virtual ~Activable() = default;

    bool IsActive() const {
        return isActive;
    }

    void SetActive(bool active) {
        this->isActive = active;
    }

protected:
    virtual void UpdateActive(float dt, float f, int mods) {}
};
