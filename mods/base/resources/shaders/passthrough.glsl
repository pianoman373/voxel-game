#pragma BEGINVERTEX
#include "postProcessingVertex.glsl"
#pragma ENDVERTEX
#pragma BEGINFRAGMENT

layout (location = 0) out vec4 outColor;

in vec2 fTexCoord;

uniform sampler2D texture0;

void main() {
    outColor = vec4(texture(texture0, fTexCoord).rgb, 1.0);
}

#pragma ENDFRAGMENT