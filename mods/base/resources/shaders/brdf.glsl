#pragma BEGINVERTEX

#include "postProcessingVertex.glsl"

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

layout (location = 0) out vec4 outColor;

in vec2 fTexCoord;

uniform sampler2D texture0;

#include "PBR.glsl"


void main() {
    vec2 integratedBRDF = IntegrateBRDF(fTexCoord.x, fTexCoord.y);
    outColor = vec4(integratedBRDF.x, integratedBRDF.y, 0.0, 1.0);
}

#pragma ENDFRAGMENT