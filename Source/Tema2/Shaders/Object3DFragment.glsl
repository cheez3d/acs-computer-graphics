#version 330

uniform vec3 Eye;
uniform vec3 Light;

in vec3 fPosition;
in vec3 fNormal;
in vec4 fColor;

layout(location = 0) out vec4 outColor;

const float kc = 0.1f;
const float kl = 0.05f;
const float kq = 0.01f;

const float kd = 1;
const float ks = 0.25f;

void main() {
    vec3 L = normalize(Light - fPosition);
    vec3 V = normalize(Eye - fPosition);

    float ambient = 0.1f;
    float diffuse = dot(L, fNormal);
    float specular = 0;

    if (diffuse > 0) {
        vec3 R = reflect(-L, fNormal);
        specular = max(0, sqrt(dot(R, V)));
    }

    float d = distance(fPosition, Light);
    float attenuation = 1 / (kc + kl * d + kq * d * d);

    outColor = vec4((ambient + max((kd * diffuse + ks * specular) * attenuation, 0)) * fColor.rgb, fColor.a);
}
