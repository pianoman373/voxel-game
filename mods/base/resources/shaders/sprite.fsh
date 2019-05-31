#version 330 core

uniform sampler2D sprite;
uniform vec4 uvOffsets;
uniform vec4 color;
uniform float textureStrength;

in vec2 fTexCoord;

out vec4 outColor;

void main() {
	outColor = mix(vec4(1.0), texture(sprite, (fTexCoord * (uvOffsets.zw - uvOffsets.xy)) + uvOffsets.xy), textureStrength) * color;

    if (outColor.w < 0.01) {
        discard;
    }
}