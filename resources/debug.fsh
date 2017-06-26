#version 330 core

uniform mat4 view;
uniform mat4 projection;

in vec3 vertexColor;

out vec4 FragColor;

void main() {
    FragColor = vec4(vertexColor, 1.0);
}
