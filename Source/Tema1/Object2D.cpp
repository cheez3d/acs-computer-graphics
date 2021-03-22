#include "Object2D.h"

#include "Scene2D.h"
#include "Tema3/Easing.h"
#include "Transform2D.h"

using BowAndArrow::Object2D;
using BowAndArrow::Scene2D;

Object2D::Object2D() {
    SetPosition({ 0, 0 });
    SetOffset({ 0, 0 });
    SetVelocity({ 0, 0 });
    SetRotation(0);
    SetScale(1);

    SetAlive(true);
    SetPhysics(false);
    SetVisible(true);

    parent = nullptr;

    SetScene(nullptr);
}

Object2D::Object2D(Object2D const& other) : Collidable(other) {
    components = other.components;

    SetPosition(other.position);
    SetOffset(other.offset);
    SetVelocity(other.velocity);
    SetRotation(other.rotation);
    SetScale(other.scale);

    SetAlive(other.isAlive);
    SetPhysics(other.hasPhysics);
    SetVisible(other.isVisible);

    parent = nullptr;

    for (auto i = 0; i < other.attachments.size(); ++i) {
        AddAttachment(i, other.attachments[i]->Clone());
    }

    SetScene(nullptr);
}

glm::vec2 Object2D::GetPosition() const {
    return position;
}

void Object2D::SetPosition(glm::vec2 position) {
    this->position = position;

    UpdateModel();
}

glm::vec2 Object2D::GetOffset() const {
    return offset;
}

void Object2D::SetOffset(glm::vec2 offset) {
    this->offset = offset;

    UpdateModel();
}

glm::vec2 Object2D::GetVelocity() const {
    return velocity;
}

void Object2D::SetVelocity(glm::vec2 velocity) {
    this->velocity = velocity;

    UpdateModel();
}

glm::vec2 Object2D::GetScale() const {
    return scale;
}

void Object2D::SetScale(glm::vec2 scale) {
    this->scale = scale;

    UpdateModel();
}

void Object2D::SetScale(float scale) {
    SetScale({ scale, scale });
}

float Object2D::GetRotation() const {
    return rotation;
}

void Object2D::SetRotation(float rotation) {
    using namespace glm;

    if (abs(rotation) > pi<float>()) {
        rotation = atan(sin(rotation), cos(rotation));
    }

    this->rotation = rotation;

    UpdateModel();
}

glm::mat3 Object2D::GetModel() const {
    return model;
}

void Object2D::UpdateModel() {
    using namespace Transform2D;

    model = Translate(position + offset) * Rotate(rotation) * Scale(scale);
}

bool Object2D::IsAlive() const {
    return isAlive;
}

void Object2D::SetAlive(bool alive) {
    isAlive = alive;
}

void Object2D::UpdateAlive(float f, int mods) {
    using namespace Easing;

    if (!isAlive) {
        if (scene->GetWorldBoundary().Contains(position)) {
            if (any(lessThan(scale, vec2 { 1 }))) {
                SetScale(Lerp(scale, vec2 { 1 }, f * 0.5f));
            }
        }
        else {
            if (any(greaterThan(scale, vec2 { 0 }))) {
                SetScale(Lerp(scale, vec2 { 0 }, f * 0.1f));
            }
        }
    }
}

bool Object2D::HasPhysics() const {
    return hasPhysics;
}

void Object2D::SetPhysics(bool physics) {
    hasPhysics = physics;
}

void Object2D::UpdatePhysics(float f, int mods) {
    if (hasPhysics) {
        SetVelocity({ velocity.x, velocity.y + f * scene->GetGravity() });
        SetPosition(position + f * velocity);
    }
}

bool Object2D::IsVisible() const {
    return isVisible;
}

void Object2D::SetVisible(bool visible) {
    isVisible = visible;
}

void Object2D::Update(float f, int mods) {
    if (!scene->IsPaused()) {
        UpdateAlive(f, mods);
        UpdatePhysics(f, mods);
    }

    if (isVisible) {
        glm::mat3 model = this->model;

        for (auto p = parent; p != nullptr; p = p->parent) {
            model = p->GetModel() * model;
        }

        for (auto&& [mesh, objectModel, userModel] : components) {
            scene->RenderMesh(mesh, "VertexColor", model * objectModel * userModel);
        }
    }

    for (auto&& attachment : attachments) {
        attachment->Update(f, mods);
    }
}

Object2D* Object2D::GetParent() const {
    return parent;
}

void Object2D::AddAttachment(int id, std::unique_ptr<Object2D> attachment) {
    Transform2D::WorldToAttachment(this, attachment.get());

    auto it = attachments.begin();
    std::advance(it, id);

    attachment->parent = this;

    attachment->SetScene(scene);

    attachments.insert(it, std::move(attachment));
}

std::unique_ptr<Object2D> Object2D::CloneAttachment(int i) const {
    auto attachment = attachments[i]->Clone();

    Transform2D::AttachmentToWorld(this, attachment.get());

    attachment->SetScene(nullptr);

    return attachment;
}

std::unique_ptr<Object2D> Object2D::RemoveAttachment(int id) {
    auto it = attachments.begin();
    std::advance(it, id);

    auto attachment = std::move(*it);

    attachments.erase(it);

    Transform2D::AttachmentToWorld(this, attachment.get());

    attachment->SetScene(nullptr);

    return attachment;
}

Scene2D* Object2D::GetScene() const {
    return scene;
}

void Object2D::SetScene(Scene2D* scene) {
    this->scene = scene;

    for (auto&& attachment : attachments) {
        attachment->SetScene(scene);
    }
}

void Object2D::UpdateCollisions(Object2D* other) {
    if (CollidesWith(other)) {
        this->CollidedWith(other);
        other->CollidedWith(this);
    }
}

std::pair<glm::vec2, float> Object2D::GetBounds() const {
    using namespace Transform2D;

    glm::mat3 model = this->model;
    glm::vec2 scale = this->scale;

    if (parent) {
        model = parent->GetModel() * model;
        scale = parent->GetScale() * scale;
    }

    return {
        model * vec3 { std::get<POSITION>(Collidable::GetBounds()), 1 },
        (Scale(scale) * vec3 { std::get<RADIUS>(Collidable::GetBounds()), 0, 1 }).x,
    };
}

bool Object2D::CollidesWith(Object2D const* other) const {
    if (!HasCollisisons() ||
        !other->HasCollisisons() ||
        attachments.empty() && std::get<RADIUS>(GetBounds()) == 0 ||
        other->attachments.empty() && std::get<RADIUS>(other->GetBounds()) == 0)
    {
        return false;
    }

    float d2 = glm::distance2(std::get<POSITION>(GetBounds()), std::get<POSITION>(other->GetBounds()));
    float r2 = pow(std::get<RADIUS>(GetBounds()) + std::get<RADIUS>(other->GetBounds()), 2);

    return d2 <= r2;
}
