#include "Scene3D.h"

using Skyroads::Scene3D;

void Scene3D::InitShaders() {
    static auto AddShaderToList = [this](Shader* shader) {
        shaders[shader->GetName()] = shader;
    };

    {
        std::string name = "Object3D";

        Shader* shader = new Shader(name.c_str());
        shader->AddShader("Source/Tema2/Shaders/Object3DVertex.glsl", GL_VERTEX_SHADER);
        shader->AddShader("Source/Tema2/Shaders/Object3DFragment.glsl", GL_FRAGMENT_SHADER);
        shader->CreateAndLink();

        AddShaderToList(shader);
    }

    {
        std::string name = "Player";

        Shader* shader = new Shader(name.c_str());
        shader->AddShader("Source/Tema2/Shaders/PlayerVertex.glsl", GL_VERTEX_SHADER);
        shader->AddShader("Source/Tema2/Shaders/Object3DFragment.glsl", GL_FRAGMENT_SHADER);
        shader->CreateAndLink();

        AddShaderToList(shader);
    }
}

std::unordered_map<std::string, Shader*>& Scene3D::GetShaders() {
    return shaders;
}
