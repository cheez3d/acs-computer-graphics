#include "Camera3D.h"

#include "Object3D.h"
#include "Scene3D.h"
#include "Transform3D.h"

using Skyroads::Camera3D;
using Skyroads::Object3D;
using Skyroads::Scene3D;

Camera3D::Camera3D() {
    SetTarget(nullptr);
    SetRotation({ 0, 0, 0 });
    SetRadius(DEFAULT_RADIUS);
    SetFov(DEFAULT_FOV);
}

Object3D* Camera3D::GetTarget() const {
    return target;
}

void Camera3D::SetTarget(Object3D* target) {
    this->target = target;

    UpdateModel();
}

glm::vec3 Camera3D::GetRotation() const {
    return rotation;
}

void Camera3D::SetRotation(glm::vec3 rotation) {
    this->rotation = rotation;

    UpdateModel();
}

void Camera3D::SetRotationX(float rotation) {
    SetRotation({ rotation, GetRotation().y, GetRotation().z });
}

void Camera3D::SetRotationY(float rotation) {
    SetRotation({ GetRotation().x, rotation, GetRotation().z });
}

void Camera3D::SetRotationZ(float rotation) {
    SetRotation({ GetRotation().x, GetRotation().y, rotation });
}

float Camera3D::GetRadius() const {
    return radius;
}

void Camera3D::SetRadius(float radius) {
    this->radius = radius;

    UpdateModel();
}

float Camera3D::GetFov() const {
    return fov;
}

void Camera3D::SetFov(float fov) {
    this->fov = fov;

    if (scene) {
        scene->UpdateProjection();
    }
}

glm::vec3 Camera3D::GetPosition() const {
    return position;
}

void Camera3D::SetPosition(glm::vec3 position) {
    this->position = position;
}

glm::mat4 Camera3D::GetView() const {
    return glm::lookAt(position, position + forward, up);
}

glm::mat4 Camera3D::GetModel() const {
    using namespace glm;

    return mat4 {
        vec4 { right,    0 },
        vec4 { up,       0 },
        vec4 { forward,  0 },
        vec4 { position, 1 },
    };
}

void Camera3D::UpdateModel() {
    using namespace Transform3D;

    mat4 m = RotateY(rotation.y) * RotateX(rotation.x) * RotateZ(rotation.z);

    right   = -m[0];
    up      =  m[1];
    forward =  m[2];

    SetPosition((target ? target->GetPosition() : vec3 { 0 }) - radius * forward);
}

Scene3D* Camera3D::GetScene() const {
    return scene;
}

void Camera3D::SetScene(Scene3D* scene) {
    this->scene = scene;

    if (scene) {
        scene->UpdateProjection();
    }
}

void Camera3D::MouseMove(int mx, int my, int dx, int dy) {
    using namespace glm;

    if (IsControllable()) {
        SetRotation({
            GetRotation().x + 0.1f * radians(static_cast<float>(dy)),
            GetRotation().y - 0.1f * radians(static_cast<float>(dx)),
            GetRotation().z,
        });
    }
}

void Camera3D::MouseScroll(int mx, int my, int ox, int oy) {
    if (IsControllable()) {
        SetRadius(GetRadius() - oy);
    }
}
