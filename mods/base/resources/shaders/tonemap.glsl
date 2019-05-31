#pragma BEGINVERTEX
#include "postProcessingVertex.glsl"
#pragma ENDVERTEX
#pragma BEGINFRAGMENT

layout (location = 0) out vec4 outColor;

in vec2 fTexCoord;

uniform sampler2D source;

vec3 aces(vec3 col, float exposure)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((col*(a*col+b))/(col*(c*col+d)+e), 0.0, 1.0);
}

vec3 reinhard(vec3 col, float exposure) {
    return vec3(1.0) - exp(-col * exposure);
}

void main() {
    vec3 color = texture(source, fTexCoord).rgb;

    // HDR tonemapping
    const float exposure = 1.0;
    color = aces(color, exposure);

    // float fogStrength = length(texture(gPosition, fTexCoord).rgb);
    // fogStrength = smoothstep(fogInner, fogOuter, fogStrength);

    // vec3 fogColor = vec3(0.7,0.74,0.82)*1.15;

	outColor = vec4(color, 1.0);
}

#pragma ENDFRAGMENT