#pragma BEGINVERTEX

#include "postProcessingVertex.glsl"

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

layout (location = 0) out vec4 outColor;

in vec2 fTexCoord;

uniform sampler2D source;

uniform sampler2D bloom0;
uniform sampler2D bloom1;
uniform sampler2D bloom2;

uniform float bloomStrength;


void main() {
    vec3 color = texture(source, fTexCoord).rgb;

    color += pow(texture(bloom0, fTexCoord).rgb, vec3(3))*bloomStrength + pow(texture(bloom1, fTexCoord).rgb, vec3(3))*bloomStrength + pow(texture(bloom2, fTexCoord).rgb, vec3(3))*bloomStrength;

	outColor = vec4(color, 1.0);
}

#pragma ENDFRAGMENT