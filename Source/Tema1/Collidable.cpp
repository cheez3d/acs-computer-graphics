#include "Collidable.h"

using BowAndArrow::Collidable;

Collidable::Collidable() {
    SetCollisions(true);

    bounds = { { 0, 0 }, 0 };
}

bool Collidable::HasCollisisons() const {
    return hasCollisions;
}

void Collidable::SetCollisions(bool collisions) {
    this->hasCollisions = collisions;
}

std::pair<glm::vec2, float> Collidable::GetBounds() const {
    return bounds;
}

void Collidable::SetBounds(std::pair<glm::vec2, float> bounds) {
    this->bounds = bounds;
}

void Collidable::SetBoundsPosition(glm::vec2 position) {
    std::get<POSITION>(bounds) = position;
}

void Collidable::SetBoundsRadius(float radius) {
    std::get<RADIUS>(bounds) = radius;
}
