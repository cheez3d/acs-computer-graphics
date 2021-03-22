#pragma once

#include <include/glm.h>

namespace Skyroads {
    class Space3D final {
        glm::vec3 position;
        glm::vec3 size;

    public:
        Space3D(glm::vec3 position, glm::vec3 size);
        Space3D();

        Space3D operator*(float f) const;

        glm::vec3 GetPosition() const;
        glm::vec3 GetSize() const;

        glm::vec3 GetMin() const;
        glm::vec3 GetMax() const;

        bool Contains(glm::vec3 point) const;
    };

    Space3D operator*(float f, Space3D const& space);
}
