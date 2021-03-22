#version 330

uniform samplerCube Skybox;

in vec3 fTexCoords;

layout (location = 0) out vec4 outColor;

void main() {
      outColor = 0.5 * texture(Skybox, fTexCoords);
}
