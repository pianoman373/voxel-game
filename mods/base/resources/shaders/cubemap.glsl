#pragma BEGINVERTEX

#include "cubemapVertex.glsl"

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

layout (location = 0) out vec4 outColor;

in vec3 fPosition;

uniform samplerCube environmentMap;

void main()
{
    outColor = vec4(texture(environmentMap, fPosition).rgb, 1.0);
}

#pragma ENDFRAGMENT