#version 330

#define LIGHTS_MAX 16

struct Light {
    vec3 Position;
    vec3 Direction;
    vec4 Color;
    float Cutoff;
};

uniform bool IsGui;
uniform sampler2D Texture;
uniform vec3 Eye;
uniform int LightsSize;
uniform Light Lights[LIGHTS_MAX];

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoord;
in vec4 fColor;

layout (location = 0) out vec4 outColor;

const float ambient = 0.05;

const float kc = 0.1;
const float kl = 0.25;
const float kq = 0.25;

const float kd = 1;
const float ks = 0.25;

void main() {
    vec4 texColor = texture(Texture, fTexCoord);

    outColor = vec4(0.4 * fColor.rgb + 0.6 * texColor.rgb, fColor.a * texColor.a);

    if (IsGui) return;

    vec3 lightsColor = vec3(0);

    for (int i = 0; i < LightsSize; ++i) {
        vec3 Light = Lights[i].Position;
        vec3 Direction = Lights[i].Direction;
        vec4 Color = Lights[i].Color;
        float Cutoff = radians(Lights[i].Cutoff);

        vec3 L = normalize(Light - fPosition);
        vec3 V = normalize(Eye - fPosition);

        float diffuse = dot(L, fNormal);
        float specular = 0.25;

        if (diffuse > 0) {
            vec3 R = reflect(-L, fNormal);
            specular = max(0, sqrt(dot(R, V)));
        }

        float light = 0;

        if (Direction == vec3(0)) {
            float d = distance(fPosition, Light);
            float att = 1 / (kc + kl * d + kq * d * d);

            light = att * max(kd * diffuse + ks * specular, 0);
        }
        else {
            float spot = dot(-L, Direction);
            float limit = cos(Cutoff);
            float att = pow(2 * (spot - limit) / (1.0 - limit), 2);

            light = (spot > limit ? att : 0) * max(kd * diffuse + ks * specular, 0);
        }

        lightsColor += light * Color.a * Color.rgb;
    }

    //lightsColor /= LIGHTS_MAX;

    outColor = vec4(lightsColor * outColor.rgb + ambient * outColor.rgb, outColor.a);
}
