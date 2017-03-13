#version 330 core

uniform sampler2D tex;

out vec4 color;

in vec2 uv;

void main() {
	color = texture(tex, uv);
}