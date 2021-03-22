#include "Scene2D.h"

#include "Object2D.h"
#include "Transform2D.h"

#include <Core/Engine.h>

#include <iostream>

using BowAndArrow::Object2D;
using BowAndArrow::Scene2D;
using BowAndArrow::Space2D;

void Scene2D::Init() {
    using namespace std::chrono;

    glClearColor(1, 1, 1, 1);

    GetCameraInput()->SetActive(false);

    auto camera = GetSceneCamera();
    camera->SetPosition({ 0, 0, 50 });
    camera->SetRotation({ 0, 0, 0, 0 });

    InitMeshes();

    SetWorld({ { -8, -4.5f }, { 16, 9 } });
    SetViewport({ { 0, 0 }, window->GetResolution() });

    gravity = -0.005f;
    time = steady_clock::now();

    SetPaused(false);
}

void Scene2D::OnInputUpdate(float dt, int mods) {
    using namespace glm;
    using namespace std::chrono;

    static auto FixId = [this](int& id) {
        if (lastObjectsRemoved.empty()) {
            return;
        }

        int oldId = id;

        for (int i : lastObjectsRemoved) {
            if (i <= oldId) {
                --id;
            }
        }

        lastObjectsRemoved.clear();
    };

    if (!isPaused) {
        time += duration_cast<steady_clock::duration>(duration<float>(dt));
    }

    float f = dt / (1.0f / 60);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!isPaused) {
        for (int i = 0; i < objects.size(); ++i) {
            Object2D* object = objects[i].get();

            if (!object->HasCollisisons()) {
                continue;
            }

            for (int j = 0; j < i; ++j) {
                Object2D* other = objects[j].get();

                object->UpdateCollisions(other);

                FixId(i);
            }
        }
    }

    for (int id = 0; id < objects.size(); ++id) {
        Object2D* object = objects[id].get();

        object->Update(f, mods);

        if (!worldBoundary.Contains(object->GetPosition())) {
            object->SetAlive(false);
            object->SetPhysics(false);
            object->SetCollisions(false);
        }

        if (!object->IsAlive() &&
            !worldBoundary.Contains(object->GetPosition()) &&
            any(lessThanEqual(object->GetScale(), vec2 { epsilon<float>() })))
        {
            RemoveObject(id);
        }

        FixId(id);
    }
}

void Scene2D::OnKeyPress(int key, int mods) {
    if (key == GLFW_KEY_P) {
        SetPaused(!isPaused);
    }

    if (isPaused) {
        return;
    }

    for (int id = 0; id < objects.size(); ++id) {
        objects[id]->KeyChange(key, mods, true);
    }
}

void Scene2D::OnKeyRelease(int key, int mods) {
    if (isPaused) {
        return;
    }

    for (int id = 0; id < objects.size(); ++id) {
        objects[id]->KeyChange(key, mods, false);
    }
}

void Scene2D::OnMouseMove(int mx, int my, int dx, int dy) {
    using namespace Transform2D;

    if (glfwGetInputMode(window->GetGLFWWindow(), GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
        ivec2 res = window->GetResolution();

        mouse = inverse(model)
              * Translate(-res / 2)
              * Translate({ 0, res.y }) * Scale({ 1, -1 })
              * vec3 { mx, my, 1 };
    }
}

void Scene2D::OnMouseBtnPress(int mx, int my, int btn, int mods) {
    if (isPaused) {
        return;
    }

    btn -= 1; // workaround for wrong `btn` reported

    for (int id = 0; id < objects.size(); ++id) {
        objects[id]->MouseChange(btn, mods, true);
    }
}

void Scene2D::OnMouseBtnRelease(int mx, int my, int btn, int mods) {
    if (isPaused) {
        return;
    }

    btn -= 1; // workaround for wrong `btn` reported

    for (int id = 0; id < objects.size(); ++id) {
        objects[id]->MouseChange(btn, mods, false);
    }
}

void Scene2D::OnWindowResize(int w, int h) {
    SetViewport({ viewport.GetPosition(), { w, h } });
}

void Scene2D::RenderMesh(std::string const& mesh, std::string const& shader, glm::mat3 model) {
    if (!meshes.count(mesh)) {
        std::cerr << "No mesh '" << mesh << "' found." << std::endl;
        return;
    }
    else if (!shaders.count(shader)) {
        std::cerr << "No shader '" << shader << "' found." << std::endl;
        return;
    }

    model = this->model * model;

    SimpleScene::RenderMesh2D(meshes[mesh], shaders[shader], model);
}

bool Scene2D::IsKeyHeld(int key) const {
    return window->KeyHold(key);
}

bool Scene2D::IsMouseHeld(int btn) const {
    return window->MouseHold(btn);
}

Space2D Scene2D::GetWorld() const {
    return world;
}

Space2D Scene2D::GetWorldBoundary() const {
    return worldBoundary;
}

void Scene2D::SetWorld(Space2D world) {
    using namespace Transform2D;

    this->world = world;

    worldBoundary = {
        Scale({ 1, 1.5f }) * vec3 { world.GetPosition() , 1 },
        Scale({ 1, 1.5f }) * vec3 { world.GetSize(), 1 },
    };

    UpdateModel();
}

Space2D Scene2D::GetViewport() const {
    return viewport;
}

void Scene2D::SetViewport(Space2D viewport) {
    this->viewport = viewport;

    glViewport(
        static_cast<int>(viewport.GetPosition().x),
        static_cast<int>(viewport.GetPosition().y),

        static_cast<int>(viewport.GetSize().x),
        static_cast<int>(viewport.GetSize().y)
    );

    auto camera = GetSceneCamera();
    camera->SetOrthographic(viewport.GetSize().x, viewport.GetSize().y, 0.01f, 400);
    camera->Update();

    UpdateModel();
}

glm::mat3 Scene2D::GetModel() const {
    return model;
}

void Scene2D::UpdateModel() {
    using namespace Transform2D;

    mat3 windowToViewport = WorldToViewport(world, viewport);
    mat3 translateToOrigin = Translate(world.GetSize() / 2.0f);

    model = windowToViewport * translateToOrigin;
}

glm::vec2 Scene2D::GetMouse() const {
    return mouse;
}

float Scene2D::GetGravity() const {
    return gravity;
}

std::chrono::time_point<std::chrono::steady_clock> Scene2D::GetTime() const {
    return time;
}

bool Scene2D::IsPaused() const {
    return isPaused;
}

void Scene2D::SetPaused(bool paused) {
    this->isPaused = paused;
}

void Scene2D::AddObject(std::unique_ptr<Object2D> object) {
    object->SetScene(this);

    objects.push_back(std::move(object));
}

std::unique_ptr<Object2D> Scene2D::RemoveObject(int id) {
    auto it = objects.begin();
    std::advance(it, id);

    auto object = std::move(*it);

    lastObjectsRemoved.push_back(id);

    objects.erase(it);

    object->SetScene(nullptr);

    return object;
}

std::unique_ptr<Object2D> Scene2D::RemoveObject(Object2D* object) {
    auto it = std::find_if(
        objects.begin(), objects.end(),
        [&](std::unique_ptr<Object2D> const& o) { return o.get() == object; }
    );

    if (it == objects.end()) {
        return nullptr;
    }

    auto o = std::move(*it);

    lastObjectsRemoved.push_back(static_cast<int>(std::distance(objects.begin(), it)));

    objects.erase(it);

    object->SetScene(nullptr);

    return o;
}
