#version 330 core

uniform sampler2D tex;
uniform vec4 uvOffsets;

in vec2 fTexCoord;
in vec3 fNormal;

out vec4 outColor;

void main() {
    vec3 color = texture(tex, (fTexCoord * (uvOffsets.zw - uvOffsets.xy)) + uvOffsets.xy).rgb * dot(normalize(fNormal), normalize(vec3(-0.3, 0.5, 0.4)));

    //color = pow(color, vec3(1.0/2.2));

    outColor = vec4(color, 1.0);
}