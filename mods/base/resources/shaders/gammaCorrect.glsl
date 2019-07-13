#pragma BEGINVERTEX

#include "postProcessingVertex.glsl"

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

layout (location = 0) out vec4 outColor;

in vec2 fTexCoord;

uniform sampler2D tex;

void main() {
    vec3 color = texture(tex, fTexCoord).rgb;

    //color = pow(color, vec3(1.0/2.2));

    outColor = vec4(color, 1.0);
}

#pragma ENDFRAGMENT