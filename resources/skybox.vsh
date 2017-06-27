#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 texCoord;

out vec3 direction;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 cameraPos;

void main() {
	direction = position;

	//gl_Position = projection * view * vec4(newpos, 1.0);
	gl_Position = projection * view * model * vec4(position, 1.0);
}
