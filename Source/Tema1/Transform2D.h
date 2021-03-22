#pragma once

#include "Object2D.h"
#include "Space2D.h"

#include <include/glm.h>

namespace BowAndArrow::Transform2D {
    using namespace glm;

    inline mat3 Translate(vec2 t) {
        mat3 m { 1, 0, t.x,
                 0, 1, t.y,
                 0, 0, 1 };

        return transpose(m);
    }

    inline mat3 Rotate(float radians) {
        float c = cos(radians);
        float s = sin(radians);

        mat3 m { c, -s, 0,
                 s,  c, 0,
                 0,  0, 1 };

        return transpose(m);
    }

    inline mat3 Scale(vec2 s) {
        mat3 m { s.x, 0,   0,
                 0,   s.y, 0,
                 0,   0,   1 };

        return m;
    }

    inline mat3 Scale(float s) {
        return Scale({ s, s });
    }

    inline mat3 WorldToViewport(Space2D const& world, Space2D const& viewport) {
        auto s = viewport.GetSize() / world.GetSize();
        float s_min = (glm::min)(s.x, s.y);

        auto t = viewport.GetPosition() + s_min * world.GetPosition();
               + (viewport.GetSize() - s_min * world.GetSize()) / 2.0f;

        mat3 m { s_min, 0,     t.x,
                 0,     s_min, t.y,
                 0,     0,     1 };

        return transpose(m);
    }

    inline void WorldToAttachment(Object2D const* parent, Object2D* attachment) {
        // does not work for nested attachments!

        vec2 position = glm::inverse(parent->GetModel())
                      * vec3 { attachment->GetPosition() + attachment->GetOffset(), 1 };

        attachment->SetPosition(position);
        attachment->SetOffset({ 0, 0 });
        attachment->SetVelocity(attachment->GetVelocity() - parent->GetVelocity());
        attachment->SetRotation(attachment->GetRotation() - parent->GetRotation());
        attachment->SetScale(attachment->GetScale() / parent->GetScale());
    }

    inline void AttachmentToWorld(Object2D const* parent, Object2D* attachment) {
        // does not work for nested attachments!

        vec2 position = parent->GetModel()
                      * vec3 { attachment->GetPosition() + attachment->GetOffset(), 1 };

        attachment->SetPosition(position);
        attachment->SetOffset({ 0, 0 });
        attachment->SetVelocity(parent->GetVelocity() + attachment->GetVelocity());
        attachment->SetRotation(parent->GetRotation() + attachment->GetRotation());
        attachment->SetScale(parent->GetScale() * attachment->GetScale());
    }
}
