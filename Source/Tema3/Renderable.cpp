#include "Renderable.h"

#include <Core/Engine.h>

Renderable::Renderable() {
    SetVisible(true);
    SetShader("Renderable");
    SetTexture("Renderable");
}

Renderable::Renderable(Renderable const& other) {
    SetVisible(other.isVisible);
    SetShader(other.shader);
    SetTexture(other.texture);
}

bool Renderable::IsVisible() const {
    return isVisible;
}

void Renderable::SetVisible(bool visible) {
    this->isVisible = visible;
}

std::string Renderable::GetShader() const {
    return shader;
}

void Renderable::SetShader(std::string shader) {
    this->shader = shader;
}

std::string Renderable::GetTexture() const {
    return texture;
}

void Renderable::SetTexture(std::string texture) {
    this->texture = texture;
}

void Renderable::UpdateShader(Shader* shader, glm::mat4 model, int componentId) const {
    int loc;

    loc = glGetUniformLocation(shader->GetProgramID(), "Model");
    glUniformMatrix4fv(loc, 1, false, value_ptr(model));
}

void Renderable::UpdateShader(Shader* shader, glm::mat4 model) const {
    UpdateShader(shader, model, 0);
}
