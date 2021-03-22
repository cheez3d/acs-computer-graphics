#include "Camera3D.h"

#include "Object3D.h"
#include "Scene3D.h"
#include "Transform3D.h"

#include <iostream>

using StylizedRunner::Camera3D;
using StylizedRunner::Object3D;
using StylizedRunner::Scene3D;

Camera3D::Camera3D() {
    SetRotation({ 0, 0, 0 });
    SetRadius(1);
    SetFov(60);

    SetObject(nullptr);
    SetScene(nullptr);
}

Camera3D::Camera3D(Camera3D const& other) {
    SetRotation(other.rotation);
    SetRadius(other.radius);
    SetFov(other.fov);

    SetObject(nullptr);
    SetScene(nullptr);
}

void Camera3D::UpdateActive(float dt, float f, int mods) {
    if (!IsActive()) {
        return;
    }

    if (object) {
        UpdateModel();
    }
}

void Camera3D::MouseMove(int dx, int dy) {
    using namespace glm;

    if (!IsControllable()) return;

    SetRotation({
        GetRotation().x + 0.1f * radians(static_cast<float>(dy)),
        GetRotation().y - 0.1f * radians(static_cast<float>(dx)),
        GetRotation().z,
    });
}

void Camera3D::MouseScroll(int ox, int oy) {
    if (!IsControllable()) return;

    SetRadius(GetRadius() - oy);
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

    UpdateProjection();
}

glm::vec3 Camera3D::GetPosition() const {
    return position;
}

void Camera3D::SetPosition(glm::vec3 position) {
    this->position = position;
}

glm::mat4 Camera3D::GetModel() const {
    return model;
}

void Camera3D::UpdateModel() {
    using namespace Transform3D;

    mat4 m = RotateY(rotation.y) * RotateX(rotation.x) * RotateZ(rotation.z);

    right   = -m[0];
    up      =  m[1];
    forward =  m[2];

    SetPosition((object ? object->GetPosition() : vec3 { 0 }) - radius * forward);

    model = mat4 {
        vec4 { right,    0 },
        vec4 { up,       0 },
        vec4 { forward,  0 },
        vec4 { position, 1 },
    };

    UpdateView();
}

glm::mat4 Camera3D::GetView() const {
    return view;
}

void Camera3D::UpdateView() {
    view = glm::lookAt(position, position + forward, up);
}

glm::mat4 Camera3D::GetProjection() const {
    return projection;
}

void Camera3D::UpdateProjection() {
    if (!scene) return;

    float ar = scene->window->props.aspectRatio;

    if (glm::isnan(ar)) {
        std::cerr << "Window dimensions are invalid."  << std::endl;

        return;
    }

    projection = glm::perspective(
        glm::radians(fov),
        scene->window->props.aspectRatio,
        0.01f, 200.0f
    );
}

Object3D* Camera3D::GetObject() const {
    return object;
}

void Camera3D::SetObject(Object3D* object) {
    this->object = object;

    if (object && object->GetCamera() != this) {
        object->SetCamera(this);
    }

    UpdateModel();
}

Scene3D* Camera3D::GetScene() const {
    return scene;
}

void Camera3D::SetScene(Scene3D* scene) {
    this->scene = scene;

    UpdateProjection();
}
