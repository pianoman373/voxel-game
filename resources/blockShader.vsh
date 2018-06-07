#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uvs;
layout (location = 2) in float aos;
layout (location = 3) in int normalIndex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 normalLookup[6];
uniform vec3 tangentLookup[6];

out vec3 fragPos;
out vec3 fragNormal;
out vec2 uv;
out vec3 fragColor;
out mat3 TBN;
out float ao;

void main()
{
    vec4 viewPos = view * model * vec4(position, 1.0);
    fragPos = viewPos.xyz;
    ao = aos;

    gl_Position = projection * viewPos;

    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    fragNormal = normalMatrix * normalLookup[normalIndex];
    uv = uvs;


    fragColor = vec3(1.0);

    vec3 T = normalize(vec3(model * vec4(tangentLookup[normalIndex], 0.0)));
    vec3 N = normalize(vec3(model * vec4(normalLookup[normalIndex], 0.0)));
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(N, T);

    TBN = mat3(view) *  mat3(T, B, N);
}