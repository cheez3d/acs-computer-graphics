#pragma once

#include <include/glm.h>

namespace BowAndArrow {
    class Space2D {
        glm::vec2 position;
        glm::vec2 size;

    public:
        Space2D(glm::vec2 position, glm::vec2 size);
        Space2D();

        Space2D operator*(float f) const;

        glm::vec2 GetPosition() const;
        glm::vec2 GetSize() const;

        glm::vec2 GetMin() const;
        glm::vec2 GetMax() const;


        bool Contains(glm::vec2 point) const;
    };

    Space2D operator*(float f, Space2D const& space);
}
