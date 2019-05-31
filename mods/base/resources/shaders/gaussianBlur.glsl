#pragma BEGINVERTEX

#include "postProcessingVertex.glsl"

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

layout (location = 0) out vec4 outColor;

in vec2 fTexCoord;

uniform sampler2D texture0;

uniform bool horizontal;
uniform float weights[64];
uniform int weights_length;

void main() {
    vec2 tex_offset = 1.0 / textureSize(texture0, 0); // gets size of single texel
    vec3 result = texture(texture0, fTexCoord).rgb * weights[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < weights_length; ++i)
        {
            result += texture(texture0, fTexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
            result += texture(texture0, fTexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
        }
    }
    else
    {
        for(int i = 1; i < weights_length; ++i)
        {
            result += texture(texture0, fTexCoord + vec2(0.0, tex_offset.y * i)).rgb * weights[i];
            result += texture(texture0, fTexCoord - vec2(0.0, tex_offset.y * i)).rgb * weights[i];
        }
    }

    result = clamp(result, 0, 100);

    outColor = vec4(result, 1.0);
}

#pragma ENDFRAGMENT