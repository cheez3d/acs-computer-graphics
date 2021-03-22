#pragma once

namespace Skyroads {
    class Controllable {
        bool isControllable;

    protected:
        Controllable() {
            SetControllable(true);
        };

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
        virtual void MouseMove(int mx, int my, int dx, int dy) {}
        virtual void MouseScroll(int mx, int my, int ox, int oy) {}
    };
}
