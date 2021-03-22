#include "Space2D.h"

using BowAndArrow::Space2D;

Space2D::Space2D(glm::vec2 position, glm::vec2 size)
    : position(position)
    , size(size)
{}

Space2D::Space2D()
    : position({ 0, 0 })
    , size({ 0, 0 })
{}

Space2D Space2D::operator*(float f) const {
    return { position * f, size * f };
}

Space2D BowAndArrow::operator*(float f, Space2D const& space) {
    return space * f;
}

glm::vec2 Space2D::GetPosition() const {
    return position;
}

glm::vec2 Space2D::GetSize() const {
    return size;
}

glm::vec2 Space2D::GetMin() const {
    return position;
}

glm::vec2 Space2D::GetMax() const {
    return position + size;
}


bool Space2D::Contains(glm::vec2 point) const {
    using namespace glm;

    return all(greaterThanEqual(point, GetMin())) &&
           all(lessThanEqual(point, GetMax()));
}
