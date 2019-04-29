#version 330 core

uniform mat4 view;
uniform mat4 projection;

in vec3 fColor;

out vec4 outColor;

void main() {
    outColor = vec4(fColor, 1.0);
}