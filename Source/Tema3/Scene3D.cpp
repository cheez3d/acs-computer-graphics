#include "Scene3D.h"

#include <Core/Engine.h>

using StylizedRunner::Camera3D;
using StylizedRunner::Light3D;
using StylizedRunner::Object3D;
using StylizedRunner::Scene3D;

#include <stb/stb_image.h>
#include <iostream>
namespace {
unsigned skyboxVAO, skyboxVBO;
unsigned skyboxId;

unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void setupBuffers() {
    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}
}

Scene3D::Scene3D() {
    SetPhysics(true);

    SetShader("Scene3D");

    SetWatch(std::make_unique<Watch<float>>());
    SetGravity({ 0, -10, 0 });

    SetCamera(nullptr);

    SetPaused(false);
}

void Scene3D::Init() {
    setupBuffers();
    skyboxId = loadCubemap({
        "Source/Tema3/Textures/panorama_1.png",
        "Source/Tema3/Textures/panorama_3.png",
        "Source/Tema3/Textures/panorama_4.png",
        "Source/Tema3/Textures/panorama_5.png",
        "Source/Tema3/Textures/panorama_0.png",
        "Source/Tema3/Textures/panorama_2.png",
    });

    InitMeshes();
    InitShaders();
    InitTextures();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    window->CenterPointer(); // initial camera twitch fix

    glClearColor(0, 0, 0, 1);

    GetCameraInput()->SetActive(false);
}

void Scene3D::OnInputUpdate(float dt, int mods) {
    this->dt = dt; // TEMP for player stop rotation on obstacle hit
    float f = dt / (1.0f / FPS);

    UpdateActive(dt, f, mods);
    UpdatePhysics(dt, f, mods);
    UpdateCollisions(dt, f, mods);
    Render(dt, f, mods);
}

void Scene3D::OnKeyPress(int key, int mods) {
    KeyChange(key, mods, true);
}

void Scene3D::OnKeyRelease(int key, int mods) {
    KeyChange(key, mods, false);
}

void Scene3D::OnMouseMove(int mx, int my, int dx, int dy) {
    if (glfwGetInputMode(window->GetGLFWWindow(), GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
        mouse = { mx, my };
    }

    MouseMove(dx, dy);
}

void Scene3D::OnMouseBtnPress(int mx, int my, int btn, int mods) {
    btn -= 1; // workaround for wrong `btn` reported

    MouseChange(btn, mods, true);
}

void Scene3D::OnMouseBtnRelease(int mx, int my, int btn, int mods) {
    btn -= 1; // workaround for wrong `btn` reported

    MouseChange(btn, mods, false);
}

void Scene3D::OnMouseScroll(int mx, int my, int ox, int oy) {
    MouseScroll(ox, oy);
}

void Scene3D::OnWindowResize(int w, int h) {
    if (w == 0 || h == 0) return;

    if (camera) {
        camera->UpdateProjection();
    }
}

void Scene3D::UpdateActive(float dt, float f, int mods) {
    if (isPaused) return;

    if (!Activable::IsActive()) return;

    watch->AddElapsed(dt);

    for (int i = 0; i < objects.size(); ++i) {
        Object3D* object = objects[i].get();

        object->UpdateActive(dt, f, mods);

        FixObjectId(i);
    }
}

void Scene3D::UpdateCollisions(float dt, float f, int mods) {
    if (isPaused) return;

    for (int i = 0; i < objects.size(); ++i) {
        Object3D* object = objects[i].get();

        if (!object->HasCollisions()) continue;

        for (int j = 0; j < i; ++j) {
            Object3D* other = objects[j].get();

            if (!object->IsDynamic() && !other->IsDynamic()) continue;

            object->UpdateCollisions(other);

            //FixObjectId(i);
        }
    }
}

void Scene3D::KeyChange(int key, int mods, bool press) {
    if (press && key == GLFW_KEY_P) {
        SetPaused(!isPaused);
    }

    if (isPaused) return;

    if (!IsControllable()) return;

    for (auto&& object : objects) {
        object->KeyChange(key, mods, press);
    }

    if (camera) {
        camera->KeyChange(key, mods, press);
    }
}

void Scene3D::MouseChange(int btn, int mods, bool press) {
    if (isPaused) return;

    if (!IsControllable()) return;

    for (auto&& object : objects) {
        object->MouseChange(btn, mods, press);
    }

    if (camera) {
        camera->MouseChange(btn, mods, press);
    }
}

void Scene3D::MouseMove(int dx, int dy) {
    if (isPaused) return;

    if (!IsControllable()) return;

    for (auto&& object : objects) {
        object->MouseMove(dx, dy);
    }

    if (camera) {
        camera->MouseMove(dx, dy);
    }
}

void Scene3D::MouseScroll(int ox, int oy) {
    if (isPaused) return;

    if (!IsControllable()) return;

    for (auto&& object : objects) {
        object->MouseScroll(ox, oy);
    }

    if (camera) {
        camera->MouseScroll(ox, oy);
    }
}

void Scene3D::UpdatePhysics(float dt, float f, int mods) {
    if (isPaused) return;

    if (!HasPhysics()) return;

    for (auto&& object : objects) {
        object->UpdatePhysics(dt, f, mods);
    }
}

void Scene3D::Render(float dt, float f, int mods) {
    using namespace glm;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!IsVisible()) return;

    if (!isPaused) {
        for (auto&& light : lights) {
            light->UpdateActive(dt, f, mods);
        }
    }

    if (!camera) return;

    camera->UpdateActive(dt, f, mods);
    
    // SKYBOX
    {
        glDepthMask(GL_FALSE);
        Shader* shader = shaders[GetShader()];
        shader->Use();
        glUniformMatrix4fv(shader->GetUniformLocation("View"), 1, GL_FALSE, value_ptr(mat4 { mat3 { camera->GetView() } }));
        glUniformMatrix4fv(shader->GetUniformLocation("Projection"), 1, GL_FALSE, value_ptr(camera->GetProjection()));

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxId);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);
    }
    // SKYBOX END

    glDepthFunc(GL_LESS);

    //DrawCoordinatSystem(camera->GetView(), camera->GetProjection());

    objectsBlending.clear();
    objectsGui.clear();

    for (auto&& object : objects) {
        if (object->IsGui()) {
            objectsGui.emplace_back(object.get());

            continue;
        }

        if (abs(1 - object->Object3D::GetAlpha()) <= epsilon<float>()) {
            object->Render(dt, f, mods);
        }
        else {
            float d2 = distance2(object->GetPosition(), camera->GetPosition());
            objectsBlending.emplace(d2, object.get());
        }
    }

    for (auto&& [d2, object] : objectsBlending) {
        object->Render(dt, f, mods);
    }

    glDepthFunc(GL_ALWAYS);

    for (auto&& object : objectsGui) {
        object->Render(dt, f, mods);
    }
}

bool Scene3D::IsKeyHeld(int key) const {
    return window->KeyHold(key);
}

bool Scene3D::IsMouseHeld(int btn) const {
    return window->MouseHold(btn);
}

Watch<float>* Scene3D::GetWatch() const {
    return watch.get();
}

void Scene3D::SetWatch(std::unique_ptr<Watch<float>> watch) {
    this->watch = std::move(watch);
}

glm::vec2 Scene3D::GetMouse() const {
    return mouse;
}

glm::vec3 Scene3D::GetGravity() const {
    return gravity;
}

void Scene3D::SetGravity(glm::vec3 gravity) {
    this->gravity = gravity;
}

Camera3D* Scene3D::GetCamera() const {
    return camera.get();
}

void Scene3D::SetCamera(std::unique_ptr<Camera3D> camera) {
    if (camera) {
        camera->SetScene(this);
    }

    this->camera = std::move(camera);
}

void Scene3D::AddObject(std::unique_ptr<Object3D> object) {
    object->SetScene(this);

    objects.push_back(std::move(object));
}

std::unique_ptr<Object3D> Scene3D::RemoveObject(int id) {
    auto it = objects.begin();
    std::advance(it, id);

    auto object = std::move(*it);

    objectIdsRemoved.push_back(id);
    objects.erase(it);

    object->SetScene(nullptr);

    // TODO temp for light cleanup
    if (object->GetLight()) {
        RemoveLight(object->GetLight());
    }

    return object;
}

std::unique_ptr<Object3D> Scene3D::RemoveObject(Object3D* object) {
    auto it = std::find_if(
        objects.begin(), objects.end(),
        [&](std::unique_ptr<Object3D> const& o) { return o.get() == object; }
    );

    if (it == objects.end()) {
        return nullptr;
    }

    return RemoveObject((int)std::distance(objects.begin(), it));
}

void Scene3D::FixObjectId(int& id) {
    if (objectIdsRemoved.empty()) {
        return;
    }

    int oldId = id;

    for (int i : objectIdsRemoved) {
        if (i <= oldId) {
            --id;
        }
    }

    objectIdsRemoved.clear();
}

void Scene3D::AddLight(std::unique_ptr<Light3D> light) {
    light->SetScene(this);

    lights.push_back(std::move(light));
}

std::unique_ptr<Light3D> Scene3D::RemoveLight(int id) {
    auto it = lights.begin();
    std::advance(it, id);

    auto light = std::move(*it);

    lights.erase(it);

    light->SetScene(nullptr);

    return light;
}

std::unique_ptr<Light3D> Scene3D::RemoveLight(Light3D* light) {
    auto it = std::find_if(
        lights.begin(), lights.end(),
        [&](std::unique_ptr<Light3D> const& l) { return l.get() == light; }
    );

    if (it == lights.end()) {
        return nullptr;
    }

    return RemoveLight((int)std::distance(lights.begin(), it));
}

bool Scene3D::IsPaused() const {
    return isPaused;
}

void Scene3D::SetPaused(bool paused) {
    if (paused) {
        window->ShowPointer();
    }
    else {
        window->DisablePointer();
    }

    this->isPaused = paused;
}
