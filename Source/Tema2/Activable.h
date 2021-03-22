#pragma once

namespace Skyroads {
    class Activable {
        bool isActive;

    protected:
        Activable() {
            SetActive(true);
        }

    public:
        virtual ~Activable() = default;

        bool IsActive() const {
            return isActive;
        }

        void SetActive(bool active) {
            this->isActive = active;
        }

    protected:
        virtual void UpdateActive(float f, int mods) {}
    };
}
