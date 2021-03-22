#include "Scene3D.h"

using Skyroads::Scene3D;

void Scene3D::InitMeshes() {
    {
        std::string name = "Box";

        Mesh* mesh = new Mesh(name);
        mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");

        AddMeshToList(mesh);
    }

    {
        std::string name = "Sphere";

        Mesh* mesh = new Mesh(name);
        mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");

        AddMeshToList(mesh);
    }
}

std::unordered_map<std::string, Mesh*>& Scene3D::GetMeshes() {
    return meshes;
}
