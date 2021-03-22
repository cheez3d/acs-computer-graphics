#include "Heart.h"

#include "Playground.h"
#include "Tema3/Random.h"
#include "Transform2D.h"

using BowAndArrow::Heart;
using BowAndArrow::Playground;

Heart::Heart() {
    using namespace Transform2D;

    components.resize(COMPONENT_COUNT);

    components[HEART] = { "RosaHeart", Scale(0.3f), mat3 { 1 } };

    size = 1;
    lastBeat = std::chrono::system_clock::from_time_t(0);
    nextBeat = std::chrono::seconds(0);
}

Heart* Heart::CloneImplementation() const {
    return new Heart(*this);
}

void Heart::UpdateAlive(float f, int mods) {
    using namespace Easing;

    Object2D::UpdateAlive(f, mods);

    if (IsAlive()) {
        auto t = std::chrono::system_clock::now();

        if (std::chrono::duration_cast<std::chrono::seconds>(t - lastBeat).count() >= nextBeat.count()) {
            size = 1.10f;
            lastBeat = t;
            nextBeat = std::chrono::seconds(Random::Int(1, 3));
        }

        SetScale(Lerp(GetScale().x, size, f * 0.5f));

        if (GetScale().x >= size - epsilon<float>()) {
            size = 1;
        }
    }
}

Playground* Heart::GetScene() const {
    return static_cast<Playground*>(Object2D::GetScene());
}

void Heart::CollidedWith(Object2D* other) {
    other->CollidedHeart(this);
}
