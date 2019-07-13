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

uniform mat4 view;

vec3 reinhard(vec3 col, float exposure) {
    return vec3(1.0) - exp(-col * exposure);
}

vec3 atmosphere(vec3 dir, vec3 sunDir) {
    float atmosphere = pow(clamp((-(dir.y*2)) / 2.0, 0.0, 1.0), 1);
    vec3 skyColor = vec3(0.2,0.4,0.8)*2;

    float sunset = clamp(-sunDir.y, 0.0, 1.0) * pow(clamp(dot(sunDir, dir), 0.0, 1.0), 5.0);
    vec3 horizonColor = mix(vec3(0.7,0.8,0.9)*2, vec3(1.7,0.8,0.9)*2, 0.0);

    return mix(horizonColor,skyColor,atmosphere) * clamp(-dir.y + 1.0, 0.2, 1.0);
}

void main() {
    vec3 color = texture(source, fTexCoord).rgb;

    vec3 viewPos = texture(gPosition, fTexCoord).rgb;

    vec3 worldPos = (inverse(view) * vec4(viewPos, 0.0)).xyz;

    vec3 atmoColor = reinhard(atmosphere(-normalize(worldPos), vec3(0.0, -1.0, 0.0)), 1.0);

    float fogStrength = length(worldPos);
    fogStrength = clamp(pow(fogStrength / fogOuter, 5), 0.0, 1.0);//smoothstep(fogInner, fogOuter, fogStrength);

    vec3 fogColor = reinhard(vec3(0.7,0.8,0.9)*2, 1.0);

	outColor = vec4(mix(color, atmoColor, fogStrength), 1.0);
}

#pragma ENDFRAGMENT