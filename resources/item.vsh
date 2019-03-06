#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;

out vec2 fTexCoord;
out vec3 fNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	fTexCoord = vTexCoord;
	fNormal = vNormal;
	gl_Position = vec4(vec2(projection * model * vec4(vPosition, 1.0)), 0.0, 1.0);
}