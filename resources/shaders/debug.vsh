#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;

uniform mat4 view;
uniform mat4 projection;

out vec3 fColor;

void main() {
	gl_Position = projection * view * vec4(vPosition, 1.0);
	fColor = vNormal;
}