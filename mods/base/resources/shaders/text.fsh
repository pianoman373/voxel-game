#version 330 core

uniform sampler2D sprite;
uniform vec4 uvOffsets;
uniform vec4 color;
uniform float textureStrength;

in vec2 fTexCoord;

out vec4 outColor;

void main() {
	vec4 texColor = texture(sprite, (fTexCoord * (uvOffsets.zw - uvOffsets.xy)) + uvOffsets.xy);
	texColor.a = texColor.r;
	texColor.rgb = vec3(1.0);

	outColor = mix(vec4(1.0), texColor, textureStrength) * color;

    if (outColor.w < 0.01) {
        discard;
    }
}