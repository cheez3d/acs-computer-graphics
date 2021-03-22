#include "Scene3D.h"

#include <Core/Engine.h>

using StylizedRunner::Scene3D;

void Scene3D::InitTextures() {
    static auto AddTextureToList = [this](Texture2D* texture) { textures[texture->GetName()] = texture; };
    std::string PATH { "Source/Tema3/Textures/" };

    {
        std::string name = "Renderable";

        Texture2D* texture = new Texture2D(name, GL_NEAREST);
        texture->Load2D(PATH + "default.png");

        AddTextureToList(texture);
    }

    {
        std::string name = "Platform";

        Texture2D* texture = new Texture2D(name, GL_NEAREST);
        texture->Load2D(PATH + "netherrack.png");

        AddTextureToList(texture);
    }

    {
        std::string name = "Player";

        Texture2D* texture = new Texture2D(name, GL_NEAREST);
        texture->Load2D(PATH + "glowstone.png");

        AddTextureToList(texture);
    }
}

std::unordered_map<std::string, Texture2D*>& Scene3D::GetTextures() {
    return textures;
}
