#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 fragNormal;
out vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;

void main() {
	fragNormal = normal;
	uv = texCoord;

	vec3 globalPos = (model * vec4(position, 1.0)).xyz;

	vec3 dir = normalize(globalPos - vec3(cameraPos.x * 64, -1024, cameraPos.z * 64));

	vec3 newpos = (dir * 64) + (dir * globalPos.y / 15);



	gl_Position = projection * view * vec4(newpos, 1.0);
}