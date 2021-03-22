#include "Space3D.h"

using Skyroads::Space3D;

Space3D::Space3D(glm::vec3 position, glm::vec3 size)
    : position(position)
    , size(size)
{}

Space3D::Space3D()
    : position({ 0, 0, 0 })
    , size({ 0, 0, 0 })
{}

Space3D Space3D::operator*(float f) const {
    return { position * f, size * f };
}

Space3D Skyroads::operator*(float f, Space3D const& space) {
    return space * f;
}

glm::vec3 Space3D::GetPosition() const {
    return position;
}

glm::vec3 Space3D::GetSize() const {
    return size;
}

glm::vec3 Space3D::GetMin() const {
    return position;
}

glm::vec3 Space3D::GetMax() const {
    return position + size;
}

bool Space3D::Contains(glm::vec3 point) const {
    using namespace glm;

    return all(greaterThanEqual(point, GetMin())) &&
           all(lessThanEqual(point, GetMax()));
}
