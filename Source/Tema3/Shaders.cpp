#include "Scene3D.h"

#include <Core/Engine.h>

using StylizedRunner::Scene3D;

void Scene3D::InitShaders() {
    static auto AddShaderToList = [this](Shader* shader) { shaders[shader->GetName()] = shader; };
    static std::string const PATH { "Source/Tema3/Shaders/" };

    {
        std::string name = "Renderable";

        Shader* shader = new Shader(name.c_str());
        shader->AddShader(PATH + "RenderableVertex.glsl", GL_VERTEX_SHADER);
        shader->AddShader(PATH + "RenderableFragment.glsl", GL_FRAGMENT_SHADER);
        shader->CreateAndLink();

        AddShaderToList(shader);
    }

    {
        std::string name = "Object3D";

        Shader* shader = new Shader(name.c_str());
        shader->AddShader(PATH + "RenderableVertex.glsl", GL_VERTEX_SHADER);
        shader->AddShader(PATH + "Object3DFragment.glsl", GL_FRAGMENT_SHADER);
        shader->CreateAndLink();

        AddShaderToList(shader);
    }

    {
        std::string name = "Scene3D";

        Shader* shader = new Shader(name.c_str());
        shader->AddShader(PATH + "Scene3DVertex.glsl", GL_VERTEX_SHADER);
        shader->AddShader(PATH + "Scene3DFragment.glsl", GL_FRAGMENT_SHADER);
        shader->CreateAndLink();

        AddShaderToList(shader);
    }

    {
        std::string name = "Player";

        Shader* shader = new Shader(name.c_str());
        shader->AddShader(PATH + "PlayerVertex.glsl", GL_VERTEX_SHADER);
        shader->AddShader(PATH + "Object3DFragment.glsl", GL_FRAGMENT_SHADER);
        shader->CreateAndLink();

        AddShaderToList(shader);
    }
}

std::unordered_map<std::string, Shader*>& Scene3D::GetShaders() {
    return shaders;
}
