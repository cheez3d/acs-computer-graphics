#version 330

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec4 Color;

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vColor;

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoord;
out vec4 fColor;

void main() {
    gl_Position = Projection * View * Model * vec4(vPosition, 1);

    fPosition = (Model * vec4(vPosition, 1)).xyz;
    fNormal = normalize(Model * vec4(vNormal, 0)).xyz;
    fTexCoord = vTexCoord;
    fColor = Color;
}
