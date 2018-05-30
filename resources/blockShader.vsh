#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 uvs;
layout (location = 3) in vec3 tangents;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 uv;
out vec3 fragColor;
out mat3 TBN;

void main()
{
    vec4 viewPos = view * model * vec4(position, 1.0);
    fragPos = viewPos.xyz;

    gl_Position = projection * viewPos;

    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    fragNormal = normalMatrix * normals;
    uv = uvs;
    fragColor = tangents;

    vec3 T = normalize(vec3(model * vec4(tangents, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normals, 0.0)));
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);
}