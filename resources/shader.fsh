#version 330 core

uniform sampler2D tex;

in vec3 fragNormal;
in vec3 fragColor;
in vec2 uv;

out vec4 color;

void main() {
	vec3 diffuse = max(dot(fragNormal, normalize(vec3(0.9, 1.1, 1.0))), 0.0) * vec3(1.5, 1.5, 1.5);
	vec3 diffuse2 = max(dot(fragNormal, normalize(vec3(-1.1, -0.9, -1.0))), 0.0) * vec3(1.0, 1.0, 1.0);

	vec3 blockColor = vec3(0.63, 0.83, 1.0);

	color = vec4(blockColor * fragColor * (diffuse + diffuse2), 1.0);
	//color = vec4((fragColor / 2), 1.0);
}