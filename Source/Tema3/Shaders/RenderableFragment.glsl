#version 330

uniform sampler2D Texture;

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoord;
in vec4 fColor;

layout (location = 0) out vec4 outColor;

void main() {
    outColor = fColor;
}
