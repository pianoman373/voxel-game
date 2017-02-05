#version 330 core

uniform sampler2D tex;

in vec3 fragNormal;
in vec2 uv;

out vec4 color;

void main() {
	float diffuse = max(dot(fragNormal, normalize(vec3(0.5, 1.0, 0.2))), 0.5);

	color = texture(tex, uv) * diffuse;
}