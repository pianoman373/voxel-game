#version 330 core

uniform sampler2DArray texArray;
uniform float textureIndex;

in vec2 fTexCoord;
in vec3 fNormal;

out vec4 outColor;

void main() {
    vec3 color = pow(texture(texArray, vec3(fTexCoord.x, fTexCoord.y, textureIndex)).rgb, vec3(2.2)) * dot(normalize(fNormal), normalize(vec3(-0.45, 0.6, 0.25))) * 1.;

    color = pow(color, vec3(1.0/2.2));

    outColor = vec4(color, 1.0);
}