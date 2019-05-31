#pragma BEGINVERTEX

#include "postProcessingVertex.glsl"

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

layout (location = 0) out vec4 outColor;

in vec2 fTexCoord;

uniform sampler2D ssaoInput;
uniform sampler2D source;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, fTexCoord + offset).r;
        }
    }
    outColor = vec4(vec3(result / (4.0 * 4.0)) * texture(source, fTexCoord).rgb, 1.0);
}

#pragma ENDFRAGMENT