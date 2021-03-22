#include "Scene3D.h"

using Skyroads::Camera3D;
using Skyroads::Object3D;
using Skyroads::Scene3D;

void Scene3D::Init() {
    using namespace std::chrono;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // initial camera twitch fix
    window->CenterPointer();

    glClearColor(0, 0, 0, 1);

    GetCameraInput()->SetActive(false);

    SetCamera(nullptr);
    SetLight({ 0, 0, 0 });

    InitMeshes();
    InitShaders();

    gravity = -0.005f;
    time = steady_clock::now();

    SetPaused(false);
}

void Scene3D::OnInputUpdate(float dt, int mods) {
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
        camera->UpdateActive(f, mods);

        for (int i = 0; i < objects.size(); ++i) {
            Object3D* object = objects[i].get();

            object->UpdateActive(f, mods);

            FixId(i);
        }
    }

    if (!isPaused) {
        for (auto&& object : objects) {
            object->UpdatePhysics(f, mods);
        }
    }

    if (!isPaused) {
        for (int i = 0; i < objects.size(); ++i) {
            Object3D* object = objects[i].get();

            if (!object->HasCollisisons()) {
                continue;
            }

            for (int j = 0; j < i; ++j) {
                Object3D* other = objects[j].get();

                object->UpdateCollisions(other);

                //FixId(i);
            }
        }
    }

    if (!isPaused && camera->GetTarget()) {
        camera->UpdateModel();
    }

    //DrawCoordinatSystem(GetCamera()->GetView(), GetProjection());

    for (auto&& object : objects) {
        if (object->GetAlpha() == 1) {
            object->Render(f, mods);
        }
    }

    for (auto&& object : objects) {
        if (object->GetAlpha() < 1) {
            object->Render(f, mods);
        }
    }

    //for (int id = 0; id < objects.size(); ++id) {
    //    Object3D* object = objects[id].get();

    //    object->Update(f, mods);

    //    //if (!worldBoundary.Contains(object->GetPosition())) {
    //    //    object->SetAlive(false);
    //    //    object->SetPhysics(false);
    //    //    object->SetCollisions(false);
    //    //}

    //    //if (!object->IsAlive() &&
    //    //    !worldBoundary.Contains(object->GetPosition()) &&
    //    //    any(lessThanEqual(object->GetScale(), vec2 { epsilon<float>() })))
    //    //{
    //    //    RemoveObject(id);
    //    //}

    //    //FixId(id);
    //}
}

void Scene3D::OnKeyPress(int key, int mods) {
    if (key == GLFW_KEY_P) {
        SetPaused(!isPaused);
    }

    if (isPaused) {
        return;
    }

    camera->KeyChange(key, mods, true);

    for (int id = 0; id < objects.size(); ++id) {
        objects[id]->KeyChange(key, mods, true);
    }
}

void Scene3D::OnKeyRelease(int key, int mods) {
    if (isPaused) {
        return;
    }

    camera->KeyChange(key, mods, false);

    for (int id = 0; id < objects.size(); ++id) {
        objects[id]->KeyChange(key, mods, false);
    }
}

void Scene3D::OnMouseMove(int mx, int my, int dx, int dy) {
    if (isPaused) {
        return;
    }

    if (glfwGetInputMode(window->GetGLFWWindow(), GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
        mouse = { mx, my };
    }

    camera->MouseMove(mx, my, dx, dy);

    for (int id = 0; id < objects.size(); ++id) {
        objects[id]->MouseMove(mx, my, dx, dy);
    }
}

void Scene3D::OnMouseBtnPress(int mx, int my, int btn, int mods) {
    if (isPaused) {
        return;
    }

    btn -= 1; // workaround for wrong `btn` reported

    camera->MouseChange(btn, mods, true);

    for (int id = 0; id < objects.size(); ++id) {
        objects[id]->MouseChange(btn, mods, true);
    }
}

void Scene3D::OnMouseBtnRelease(int mx, int my, int btn, int mods) {
    if (isPaused) {
        return;
    }

    btn -= 1; // workaround for wrong `btn` reported

    camera->MouseChange(btn, mods, false);

    for (int id = 0; id < objects.size(); ++id) {
        objects[id]->MouseChange(btn, mods, false);
    }
}

void Scene3D::OnMouseScroll(int mx, int my, int ox, int oy) {
    if (isPaused) {
        return;
    }
    
    camera->MouseScroll(mx, my, ox, oy);

    for (auto id = 0; id < objects.size(); ++id) {
        objects[id]->MouseScroll(mx, my, ox, oy);
    }
}

void Scene3D::OnWindowResize(int w, int h) {
    if (w != 0 && h != 0) {
        UpdateProjection();
    }
}

bool Scene3D::IsKeyHeld(int key) const {
    return window->KeyHold(key);
}

bool Scene3D::IsMouseHeld(int btn) const {
    return window->MouseHold(btn);
}

Camera3D* Scene3D::GetCamera() const {
    return camera.get();
}

void Scene3D::SetCamera(std::unique_ptr<Camera3D> camera) {
    this->camera = std::move(camera);

    if (this->camera) {
        this->camera->SetScene(this);
    }
}

glm::vec3 Scene3D::GetLight() const {
    return light;
}

void Scene3D::SetLight(glm::vec3 light) {
    this->light = light;
}

glm::mat4 Scene3D::GetProjection() const {
    return projection;
}

void Scene3D::UpdateProjection() {
    if (camera) {
        projection = glm::perspective(
            glm::radians(camera->Camera3D::GetFov()),
            window->props.aspectRatio,
            0.01f, 200.0f
        );
    }
}

float Scene3D::GetGravity() const {
    return gravity;
}

std::chrono::time_point<std::chrono::steady_clock> Scene3D::GetTime() const {
    return time;
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

void Scene3D::AddObject(std::unique_ptr<Object3D> object) {
    object->SetScene(this);
    
    objects.push_back(std::move(object));
}

std::unique_ptr<Object3D> Scene3D::RemoveObject(int id) {
    auto it = objects.begin();
    std::advance(it, id);

    auto object = std::move(*it);

    lastObjectsRemoved.push_back(id);

    objects.erase(it);

    object->SetScene(nullptr);

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

    auto o = std::move(*it);

    lastObjectsRemoved.push_back(static_cast<int>(std::distance(objects.begin(), it)));

    objects.erase(it);

    object->SetScene(nullptr);

    return o;
}
