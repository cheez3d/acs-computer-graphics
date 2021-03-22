#version 330

layout (location = 0) in vec3 vPosition;

out vec3 fTexCoords;

uniform mat4 View;
uniform mat4 Projection;

void main() {
    gl_Position = Projection * View * vec4(vPosition, 1);

    fTexCoords = vPosition;
}
