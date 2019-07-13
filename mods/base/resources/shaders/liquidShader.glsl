#pragma BEGINVERTEX

layout (location = 0) in vec3 position;
layout (location = 1) in float aos;
layout (location = 2) in int normalIndex;
layout (location = 3) in vec3 uvs;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 normalLookup[6];
uniform vec3 tangentLookup[6];

out vec3 fragPos;
out vec3 fragNormal;
out vec3 uv;
out vec3 fragColor;
out mat3 TBN;
out float ao;
out vec4 screenPos;

void main()
{
    vec4 viewPos = view * model * vec4(position, 1.0);
    fragPos = viewPos.xyz;
    ao = pow(aos, 3);

    gl_Position = projection * viewPos;
    screenPos = gl_Position;

    // mat3 normalMatrix = transpose(inverse(mat3()));
    fragNormal = mat3(view * model) * normalLookup[normalIndex];
    uv = uvs;


    fragColor = vec3(1.0);

    // vec3 T = normalize(vec3(model * vec4(tangentLookup[normalIndex], 0.0)));
    // vec3 N = normalize(vec3(model * vec4(normalLookup[normalIndex], 0.0)));
    // // re-orthogonalize T with respect to N
    // T = normalize(T - dot(T, N) * N);
    // // then retrieve perpendicular vector B with the cross product of T and N
    // vec3 B = cross(N, T);

    // TBN = mat3(view) *  mat3(T, B, N);
}

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

#include "PBR.glsl"

layout (location = 0) out vec4 outColor;

in vec3 fragPos;
in vec3 fragNormal;
in vec3 uv;
in vec3 fragColor;
in float ao;
in vec4 screenPos;

uniform sampler2D gPosition;
uniform sampler2D gAlbedo;
uniform sampler2D gNormal;

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

uniform DirectionalLight sun;

void main()
{
  vec2 screenUV = (screenPos.xy / screenPos.z) * 0.5f + 0.5f;

  vec3 color = vec3(0.3, 0.3, 1.0)*0.3;

  vec3 light = directionalLighting(normalize(vec3(0.4, -0.8, 0.2)), vec3(1.0), fragPos, color, fragNormal, 0.1, 0.0);

  outColor = vec4(color, 0.9);
}

#pragma ENDFRAGMENT