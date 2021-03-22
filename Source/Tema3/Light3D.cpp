#include "Light3D.h"

#include "Object3D.h"

using StylizedRunner::Light3D;
using StylizedRunner::Object3D;
using StylizedRunner::Scene3D;

Light3D::Light3D() {
    SetDirection({ 0, 0, 0 });
    SetCutoff(0);
    SetColor({ 1, 1, 1 });
    SetAlpha(1);

    SetObject(nullptr);
    SetScene(nullptr);
}

Light3D::Light3D(Light3D const& other) {
    SetDirection(other.direction);
    SetCutoff(other.cutoff);
    SetColor(other.color);
    SetAlpha(other.alpha);

    SetObject(nullptr);
    SetScene(nullptr);
}

void Light3D::UpdateActive(float dt, float f, int mods) {
    if (!IsActive()) return;

    if (!object) return;

    UpdateModel();
}

glm::vec3 Light3D::GetDirection() const {
    return direction;
}

void Light3D::SetDirection(glm::vec3 direction) {
    this->direction = direction;
}

float Light3D::GetCutoff() const {
    return cutoff;
}

void Light3D::SetCutoff(float cutoff) {
    this->cutoff = cutoff;
}

glm::vec3 Light3D::GetColor() const {
    return color;
}

void Light3D::SetColor(glm::vec3 color) {
    this->color = color;
}

float Light3D::GetAlpha() const {
    return alpha;
}

void Light3D::SetAlpha(float alpha) {
    this->alpha = alpha;
}

glm::vec3 Light3D::GetPosition() const {
    return position;
}

void Light3D::SetPosition(glm::vec3 position) {
    this->position = position;
}

glm::mat4 Light3D::GetModel() const {
    return model;
}
#include "Player.h"
void Light3D::UpdateModel() {
    using namespace Transform3D;

    vec3 offset { 0 };

    // temp hack
    if (object) {
        if (dynamic_cast<Player*>(object)) {
            offset += vec3 { 0, 0, -0.5f };
            SetDirection(normalize(vec3 { object->GetPosition().x, 0.5f, -4 } - object->GetPosition() + offset));
        }
        else {
            offset += vec3 { 0, 1 * object->GetScale().y, 0 };
            SetAlpha(object ? object->GetAlpha() : 0);
        }
        SetColor(object ? object->GetColor() : vec3 { 0 });
    }
    // temp hack end

    SetPosition(object ? object->GetPosition() + offset : vec3 { 0 });

    model = Translate(position);
}

Object3D* Light3D::GetObject() const{
    return object;
}

void StylizedRunner::Light3D::SetObject(Object3D* object) {
    this->object = object;

    if (object && object->GetLight() != this) {
        object->SetLight(this);
    }

    UpdateModel();
}

Scene3D* Light3D::GetScene() const {
    return scene;
}

void StylizedRunner::Light3D::SetScene(Scene3D* scene) {
    this->scene = scene;
}
