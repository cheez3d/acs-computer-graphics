#include "Scene3D.h"

#include <Core/Engine.h>

using StylizedRunner::Scene3D;

void Scene3D::InitMeshes() {
    static std::string const PATH { RESOURCE_PATH::MODELS + "Primitives" };

    {
        std::string name = "Box";

        Mesh* mesh = new Mesh(name);
        mesh->LoadMesh(PATH, "box.obj");
        mesh->UseMaterials(false);

        AddMeshToList(mesh);
    }

    {
        std::string name = "Sphere";

        Mesh* mesh = new Mesh(name);
        mesh->LoadMesh(PATH, "sphere.obj");
        mesh->UseMaterials(false);

        AddMeshToList(mesh);
    }
}

std::unordered_map<std::string, Mesh*>& Scene3D::GetMeshes() {
    return meshes;
}
