#include "Object3D.h"

#include "Scene3D.h"
#include "Transform3D.h"

#include <iostream>

using Skyroads::Object3D;
using Skyroads::Scene3D;

void Object3D::AttachmentToParent(Object3D const* parent, Object3D* attachment) {
    using namespace glm;

    // does not work for nested attachments!

    vec3 position = glm::inverse(parent->GetModel()) * vec4 { attachment->GetPosition(), 1 };

    attachment->SetPosition(position);
    attachment->SetVelocity(attachment->GetVelocity() - parent->GetVelocity());
    attachment->SetRotation(attachment->GetRotation() - parent->GetRotation());
    attachment->SetScale(attachment->GetScale() / parent->GetScale());
}

void Object3D::AttachmentToWorld(Object3D const* parent, Object3D* attachment) {
    using namespace glm;

    // does not work for nested attachments!

    vec3 position = parent->GetModel() * vec4 { attachment->GetPosition(), 1 };

    attachment->SetPosition(position);
    attachment->SetVelocity(parent->GetVelocity() + attachment->GetVelocity());
    attachment->SetRotation(parent->GetRotation() + attachment->GetRotation());
    attachment->SetScale(parent->GetScale() * attachment->GetScale());
}

Object3D::Object3D() {
    GetBounding()->SetParent(this);

    SetShader("Object3D");

    SetPosition({ 0, 0, 0 });
    SetVelocity({ 0, 0, 0 });
    SetRotation({ 0, 0, 0 });
    SetScale(1);
    SetColor({ 0.5, 0.5, 0.5 });
    SetAlpha(1);

    SetActive(true);
    SetPhysics(false);
    SetVisible(true);

    parent = nullptr;

    SetScene(nullptr);
}

Object3D::Object3D(Object3D const& other)
    : Activable(other)
    , Collidable(other)
{
    GetBounding()->SetParent(this);

    components = other.components;

    SetShader(other.shader);

    SetPosition(other.position);
    SetVelocity(other.velocity);
    SetRotation(other.rotation);
    SetScale(other.scale);
    SetColor(other.color);
    SetAlpha(other.alpha);

    SetPhysics(other.hasPhysics);
    SetVisible(other.isVisible);

    parent = nullptr;

    for (auto i = 0; i < other.attachments.size(); ++i) {
        AddAttachment(i, other.attachments[i]->Clone());
    }

    SetScene(nullptr);
}

void Object3D::SetBounding(std::unique_ptr<Bounding> bounding) {
    bounding->SetParent(this);

    Collidable::SetBounding(std::move(bounding));
}

std::string Object3D::GetShader() const {
    return shader;
}

void Object3D::SetShader(std::string shader) {
    this->shader = shader;
}

glm::vec3 Object3D::GetPosition() const {
    return position;
}

void Object3D::SetPosition(glm::vec3 position) {
    this->position = position;

    UpdateModel();
}

glm::vec3 Object3D::GetVelocity() const {
    return velocity;
}

void Object3D::SetVelocity(glm::vec3 velocity) {
    this->velocity = velocity;

    UpdateModel();
}

glm::vec3 Object3D::GetRotation() const {
    return rotation;
}

void Object3D::SetRotation(glm::vec3 rotation) {
    using namespace glm;

    if (any(greaterThan(abs(rotation), glm::vec3(pi<float>())))) {
        rotation = atan(sin(rotation), cos(rotation));
    }

    this->rotation = rotation;

    UpdateModel();
}

void Object3D::SetRotationX(float rotation) {
    SetRotation({ rotation, this->rotation.y, this->rotation.z });
}

void Object3D::SetRotationY(float rotation) {
    SetRotation({ this->rotation.x, rotation, this->rotation.z });
}

void Object3D::SetRotationZ(float rotation) {
    SetRotation({ this->rotation.x, this->rotation.y, rotation });
}

glm::vec3 Object3D::GetScale() const {
    return scale;
}

void Object3D::SetScale(glm::vec3 scale) {
    this->scale = scale;

    UpdateModel();
}

void Object3D::SetScale(float scale) {
    SetScale({ scale, scale, scale });
}

glm::vec3 Object3D::GetColor() const {
    return color;
}

void Object3D::SetColor(glm::vec3 color) {
    this->color = color;
}

float Object3D::GetAlpha() const {
    return alpha;
}

void Object3D::SetAlpha(float alpha) {
    this->alpha = alpha;
}

glm::mat4 Object3D::GetModel() const {
    return model;
}

void Object3D::UpdateModel() {
    using namespace Transform3D;

    model = Translate(position)
          * RotateZ(rotation.z) * RotateY(rotation.y) * RotateX(rotation.x)
          * Scale(scale);
}

void Object3D::UpdateActive(float f, int mods) {
    // TODO

    for (auto&& attachment : attachments) {
        attachment->UpdateActive(f, mods);
    }
}

bool Object3D::HasPhysics() const {
    return hasPhysics;
}

void Object3D::SetPhysics(bool physics) {
    hasPhysics = physics;
}

void Object3D::UpdatePhysics(float f, int mods) {
    using namespace glm;

    if (hasPhysics) {
        SetVelocity(velocity + f * vec3 { 0, scene->GetGravity(), 0 });
        SetPosition(position + f * velocity);
    }
}

bool Object3D::IsVisible() const {
    return isVisible;
}

void Object3D::SetVisible(bool visible) {
    isVisible = visible;
}

void Object3D::PrepareShader(Shader* shader, glm::mat4 model, int component) const {
    using namespace glm;

    int loc;

    loc = glGetUniformLocation(shader->GetProgramID(), "Model");
    glUniformMatrix4fv(loc, 1, false, value_ptr(model));

    loc = glGetUniformLocation(shader->GetProgramID(), "View");
    glUniformMatrix4fv(loc, 1, false, value_ptr(scene->GetCamera()->GetView()));

    loc = glGetUniformLocation(shader->GetProgramID(), "Projection");
    glUniformMatrix4fv(loc, 1, false, value_ptr(scene->GetProjection()));

    loc = glGetUniformLocation(shader->GetProgramID(), "Time");
    glUniform1f(loc, Engine::GetElapsedTime());

    loc = glGetUniformLocation(shader->GetProgramID(), "Component");
    glUniform1i(loc, component);

    loc = glGetUniformLocation(shader->GetProgramID(), "Color");
    glUniform4fv(loc, 1, value_ptr(vec4 { color, alpha }));

    loc = glGetUniformLocation(shader->GetProgramID(), "Eye");
    glUniform3fv(loc, 1, value_ptr(scene->GetCamera()->GetPosition()));

    loc = glGetUniformLocation(shader->GetProgramID(), "Light");
    glUniform3fv(loc, 1, value_ptr(scene->Scene3D::GetLight()));
}

void Object3D::Render(float f, int mods) {
    using namespace glm;

    if (isVisible) {
        auto&& meshes = scene->GetMeshes();
        auto&& shaders = scene->GetShaders();

        glm::mat4 model = this->model;

        for (auto p = parent; p != nullptr; p = p->parent) {
            model = p->GetModel() * model;
        }

        for (int i = 0; i < components.size(); ++i) {
            auto&& [mesh, objectModel, userModel] = components[i];

            if (!meshes.count(mesh)) {
                std::cerr << "No mesh '" << mesh << "' found." << std::endl;
                return;
            }
            else if (!shaders.count(shader)) {
                std::cerr << "No shader '" << shader << "' found." << std::endl;
                return;
            }

            shaders[shader]->Use();

            PrepareShader(shaders[shader], model * objectModel * userModel, i);

            meshes[mesh]->Render();
        }
    }

    for (auto&& attachment : attachments) {
        attachment->Render(f, mods);
    }
}

Object3D* Object3D::GetParent() const {
    return parent;
}

void Object3D::AddAttachment(int id, std::unique_ptr<Object3D> attachment) {
    AttachmentToParent(this, attachment.get());

    auto it = attachments.begin();
    std::advance(it, id);

    attachment->parent = this;

    attachment->SetScene(scene);

    attachments.insert(it, std::move(attachment));
}

std::unique_ptr<Object3D> Object3D::CloneAttachment(int i) const {
    auto attachment = attachments[i]->Clone();

    AttachmentToWorld(this, attachment.get());

    attachment->SetScene(nullptr);

    return attachment;
}

std::unique_ptr<Object3D> Object3D::RemoveAttachment(int id) {
    auto it = attachments.begin();
    std::advance(it, id);

    auto attachment = std::move(*it);

    attachments.erase(it);

    AttachmentToWorld(this, attachment.get());

    attachment->SetScene(nullptr);

    return attachment;
}

Scene3D* Object3D::GetScene() const {
    return scene;
}

void Object3D::SetScene(Scene3D* scene) {
    this->scene = scene;

    for (auto&& attachment : attachments) {
        attachment->SetScene(scene);
    }
}

void Object3D::UpdateCollisions(Object3D* other) {
    if (CollidesWith(other)) {
        this->CollidedWith(other);
        other->CollidedWith(this);
    }
}
