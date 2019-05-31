#pragma BEGINVERTEX

#include "postProcessingVertex.glsl"

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

layout (location = 0) out vec4 outColor;

in vec2 fTexCoord;

uniform sampler2D source;
uniform sampler2D gPosition;

uniform float fogInner;
uniform float fogOuter;

void main() {
    vec3 color = texture(source, fTexCoord).rgb;

    float fogStrength = length(texture(gPosition, fTexCoord).rgb);
    fogStrength = smoothstep(fogInner, fogOuter, fogStrength);

    vec3 fogColor = vec3(0.7,0.74,0.82);
    fogColor = pow(fogColor, vec3(2.2));

	outColor = vec4(mix(color, fogColor, fogStrength), 1.0);
}

#pragma ENDFRAGMENT