#pragma once

#include <include/glm.h>

#include <string>

class Shader;

class Renderable {
protected:
private:
    bool isVisible;
    std::string shader;
    std::string texture;

protected:
    Renderable();

    Renderable(Renderable const& other);
    Renderable& operator=(Renderable const& other) = delete;

    Renderable(Renderable&& other) = default;
    Renderable& operator=(Renderable&& other) = default;

public:
    virtual ~Renderable() = default;

    bool IsVisible() const;
    std::string GetShader() const;
    std::string GetTexture() const;

    void SetVisible(bool visible);
    void SetShader(std::string shader);
    void SetTexture(std::string texture);

protected:
    virtual void UpdateShader(Shader* shader, glm::mat4 model, int componentId) const;
    void UpdateShader(Shader* shader, glm::mat4 model) const;
    virtual void Render(float dt, float f, int mods) {}
};
