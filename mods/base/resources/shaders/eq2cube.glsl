#pragma BEGINVERTEX

#include "cubemapVertex.glsl"

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

out vec4 outColor;

in vec3 fPosition;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(fPosition)); // make sure to normalize localPos
    vec3 color = texture(equirectangularMap, uv).rgb;

    outColor = vec4(color, 1.0);
}

#pragma ENDFRAGMENT