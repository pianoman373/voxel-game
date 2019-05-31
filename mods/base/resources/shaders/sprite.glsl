#pragma BEGINVERTEX

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;

out vec2 fTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	fTexCoord = vec2(vTexCoord.x, 1-vTexCoord.y);
	gl_Position = vec4(vec2(projection * model * vec4(vPosition, 1.0)), 0.0, 1.0);
}

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

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

#pragma ENDFRAGMENT