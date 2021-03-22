#include "Object3D.h"

#include "Camera3D.h"
#include "Scene3D.h"
#include "Transform3D.h"

#include <Core/Engine.h>

#include <iostream>

using StylizedRunner::Camera3D;
using StylizedRunner::Light3D;
using StylizedRunner::Object3D;
using StylizedRunner::Scene3D;

void Object3D::AttachmentToParent(Object3D const* parent, Object3D* attachment) {
    using namespace glm;

    // NB does not work for nested attachments!

    vec3 position = glm::inverse(parent->GetModel()) * vec4 { attachment->GetPosition(), 1 };

    attachment->SetPosition(position);
    attachment->SetVelocity(attachment->GetVelocity() - parent->GetVelocity());
    attachment->SetRotation(attachment->GetRotation() - parent->GetRotation());
    attachment->SetScale(attachment->GetScale() / parent->GetScale());
}

void Object3D::AttachmentToWorld(Object3D const* parent, Object3D* attachment) {
    using namespace glm;

    // NB does not work for nested attachments!

    vec3 position = parent->GetModel() * vec4 { attachment->GetPosition(), 1 };

    attachment->SetPosition(position);
    attachment->SetVelocity(parent->GetVelocity() + attachment->GetVelocity());
    attachment->SetRotation(parent->GetRotation() + attachment->GetRotation());
    attachment->SetScale(parent->GetScale() * attachment->GetScale());
}

Object3D::Object3D() {
    SetActive(true);
    GetBounding()->SetParent(this);

    SetShader("Object3D");

    SetPosition({ 0, 0, 0 });
    SetVelocity({ 0, 0, 0 });
    SetRotation({ 0, 0, 0 });
    SetScale(1);
    SetColor({ 0.5, 0.5, 0.5 });
    SetAlpha(1);

    SetCamera(nullptr);
    SetLight(nullptr);
    SetScene(nullptr);

    SetParent(nullptr);
}

Object3D::Object3D(Object3D const& other)
    : Activable(other)
    , Collidable(other)
    , Controllable(other)
    , Physics(other)
    , Renderable(other)
{
    GetBounding()->SetParent(this);

    components = other.components;

    SetPosition(other.position);
    SetVelocity(other.velocity);
    SetRotation(other.rotation);
    SetScale(other.scale);
    SetColor(other.color);
    SetAlpha(other.alpha);

    SetCamera(nullptr);
    SetLight(nullptr);
    SetScene(nullptr);

    for (int i = 0; i < other.attachments.size(); ++i) {
        AddAttachment(i, other.attachments[i]->Clone());
    }

    SetParent(nullptr);
}

void Object3D::UpdateActive(float dt, float f, int mods) {
    for (auto&& attachment : attachments) {
        attachment->UpdateActive(dt, f, mods);
    }
}

void Object3D::UpdateCollisions(Object3D* other) {
    if (!HasCollisions() || !other->HasCollisions()) return;

    if (!CollidesWith(other)) return;

    this->CollidedWith(other);
    other->CollidedWith(this);
}

void Object3D::SetBounding(std::unique_ptr<Bounding> bounding) {
    Collidable::SetBounding(std::move(bounding));

    GetBounding()->SetParent(this);
}

void Object3D::UpdatePhysics(float dt, float f, int mods) {
    if (!HasPhysics()) {
        return;
    }

    SetVelocity(velocity + dt * (1 - GetDrag()) * scene->GetGravity());
    SetPosition(position + dt * velocity);
}
#include "PointLight.h"
void Object3D::UpdateShader(Shader* shader, glm::mat4 model, int componentId) const {
    using namespace glm;

    Renderable::UpdateShader(shader, model, componentId);

    auto&& textures = scene->GetTextures();

    std::string uniform;
    int loc;

    loc = glGetUniformLocation(shader->GetProgramID(), "IsGui");
    glUniform1i(loc, IsGui());

    auto view = IsGui() ? mat4 { 1 } : scene->GetCamera()->GetView();
    loc = glGetUniformLocation(shader->GetProgramID(), "View");
    glUniformMatrix4fv(loc, 1, false, value_ptr(view));

    auto projection = IsGui() ? GUI_PROJECTION : scene->GetCamera()->GetProjection();
    loc = glGetUniformLocation(shader->GetProgramID(), "Projection");
    glUniformMatrix4fv(loc, 1, false, value_ptr(projection));

    loc = glGetUniformLocation(shader->GetProgramID(), "Time");
    glUniform1f(loc, scene->GetWatch()->GetElapsed());

    loc = glGetUniformLocation(shader->GetProgramID(), "ComponentId");
    glUniform1i(loc, componentId);

    loc = glGetUniformLocation(shader->GetProgramID(), "Color");
    glUniform4fv(loc, 1, value_ptr(vec4 { color, alpha }));

    if (!textures.count(GetTexture())) {
        std::cerr << "No texture '" << GetTexture() << "' found." << std::endl;

        return;
    }

    auto texture = textures[GetTexture()];
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
    loc = glGetUniformLocation(shader->GetProgramID(), "Texture");
    glUniform1i(loc, 0);

    loc = glGetUniformLocation(shader->GetProgramID(), "Eye");
    glUniform3fv(loc, 1, value_ptr(scene->GetCamera()->GetPosition()));

    loc = glGetUniformLocation(shader->GetProgramID(), "LightsSize");
    glUniform1i(loc, (int)scene->lights.size());

    for (int i = 0; i < scene->lights.size(); ++i) {
        auto&& light = scene->lights[i];

        uniform = "Lights[" + std::to_string(i) + "].Position";
        loc = glGetUniformLocation(shader->GetProgramID(), uniform.c_str());
        glUniform3fv(loc, 1, value_ptr(light->GetPosition()));

        uniform = "Lights[" + std::to_string(i) + "].Direction";
        loc = glGetUniformLocation(shader->GetProgramID(), uniform.c_str());
        glUniform3fv(loc, 1, value_ptr(light->GetDirection()));

        uniform = "Lights[" + std::to_string(i) + "].Color";
        loc = glGetUniformLocation(shader->GetProgramID(), uniform.c_str());
        glUniform4fv(loc, 1, value_ptr(vec4 { light->GetColor(), light->GetAlpha() }));

        uniform = "Lights[" + std::to_string(i) + "].Cutoff";
        loc = glGetUniformLocation(shader->GetProgramID(), uniform.c_str());
        glUniform1f(loc, light->GetCutoff());
    }
}

void Object3D::Render(float dt, float f, int mods) {
    using namespace glm;

    for (auto&& attachment : attachments) {
        attachment->Render(dt, f, mods);
    }

    if (!IsVisible()) {
        return;
    }

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

            continue;
        }
        else if (!shaders.count(GetShader())) {
            std::cerr << "No shader '" << GetShader() << "' found." << std::endl;

            return;
        }

        shaders[GetShader()]->Use();

        UpdateShader(shaders[GetShader()], model * objectModel * userModel, i);

        //meshes[mesh]->Render();
        glBindVertexArray(meshes[mesh]->GetBuffers()->VAO);
        glDrawElements(meshes[mesh]->GetDrawMode(), static_cast<int>(meshes[mesh]->indices.size()), GL_UNSIGNED_SHORT, 0);
    }
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

Camera3D* Object3D::GetCamera() const {
    return camera;
}

void Object3D::SetCamera(Camera3D* camera) {
    this->camera = camera;

    if (camera && camera->GetObject() != this) {
        camera->SetObject(this);
    }
}

Light3D* Object3D::GetLight() const {
    return light;
}

void Object3D::SetLight(Light3D* light) {
    this->light = light;

    if (light && light->GetObject() != this) {
        light->SetObject(this);
    }
}

Scene3D* Object3D::GetScene() const {
    return scene;
}

void Object3D::SetScene(Scene3D* scene) {
    for (auto&& attachment : attachments) {
        attachment->SetScene(scene);
    }

    this->scene = scene;
}

void Object3D::AddAttachment(int id, std::unique_ptr<Object3D> attachment) {
    AttachmentToParent(this, attachment.get());

    auto it = attachments.begin();
    std::advance(it, id);

    attachment->SetScene(scene);

    attachment->SetParent(this);

    attachments.insert(it, std::move(attachment));
}

std::unique_ptr<Object3D> Object3D::CloneAttachment(int id) const {
    auto attachment = attachments[id]->Clone();

    AttachmentToWorld(this, attachment.get());

    attachment->SetScene(nullptr);

    return attachment;
}

std::unique_ptr<Object3D> Object3D::RemoveAttachment(int id) {
    auto it = attachments.begin();
    std::advance(it, id);

    std::unique_ptr<Object3D> attachment = std::move(*it);

    attachments.erase(it);

    AttachmentToWorld(this, attachment.get());

    attachment->SetScene(nullptr);

    return attachment;
}

Object3D* Object3D::GetParent() const {
    return parent;
}

void Object3D::SetParent(Object3D* parent) {
    this->parent = parent;
}
