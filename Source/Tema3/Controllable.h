#pragma once

class Controllable {
    bool isControllable;

protected:
    Controllable() {
        SetControllable(true);
    };

    Controllable(Controllable const& other) {
        SetControllable(other.isControllable);
    }

    Controllable& operator=(Controllable const& other) = delete;

    Controllable(Controllable&& other) = default;
    Controllable& operator=(Controllable&& other) = default;

public:
    virtual ~Controllable() = default;

    bool IsControllable() const {
        return isControllable;
    }

    void SetControllable(bool controllable) {
        this->isControllable = controllable;
    }

protected:
    virtual void KeyChange(int key, int mods, bool press) {}
    virtual void MouseChange(int btn, int mods, bool press) {}
    virtual void MouseMove(int dx, int dy) {}
    virtual void MouseScroll(int ox, int oy) {}
};
