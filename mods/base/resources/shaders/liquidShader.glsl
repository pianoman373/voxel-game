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
out vec4 screenPos;
out vec3 fragNormal;
out vec3 uv;
out vec3 fragColor;
out mat3 TBN;
out float ao;

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

layout (location = 0) out vec4 outColor;

in vec3 fragPos;
in vec4 screenPos;
in vec3 fragNormal;
in vec3 uv;
in vec3 fragColor;
in float ao;

uniform sampler2D gPosition;
uniform sampler2D gAlbedo;
uniform sampler2D gNormal;

void main()
{
  vec3 color = vec3(0.3, 0.3, 1.0)*0.3;

  outColor = vec4(color, 0.9);
}

#pragma ENDFRAGMENT