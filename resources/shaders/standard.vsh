#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in ivec4 vBoneIDs;
layout (location = 5) in vec4 vBoneWeights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 bones[100];

uniform bool doAnimation;

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoord;
out mat3 TBN;
out vec3 debug;

void main()
{
    vec4 viewPos;

    mat3 normalMatrix = transpose(inverse(mat3(view * model)));

    // if (doAnimation) {
    //     vec4 totalLocalPos = vec4(0.0);
    //     vec3 totalLocalNormal = vec3(0.0);

    //     for(int i=0;i<4;i++){
    //         mat4 jointTransform = bones[vBoneIDs[i]];
    //         vec4 posePosition = jointTransform * vec4(vPosition, 1.0);
    //         totalLocalPos += posePosition * (vBoneWeights[i] + 0.000000001);
    //         totalLocalNormal += vec3(jointTransform * vec4(normalize(vNormal), 0.0)) * (vBoneWeights[i] + 0.000000001);
    //     }

    //     viewPos = view * model * totalLocalPos;
    //     fNormal = normalMatrix * totalLocalNormal;
    // }
    // else {
        
    // }
    viewPos = view * model * vec4(vPosition, 1.0);
    fNormal = normalMatrix * normalize(vNormal);

    fPosition = viewPos.xyz;

    gl_Position = projection * viewPos;

    fTexCoord = vec2(vTexCoord.x, 1-vTexCoord.y);

    // calculate TBN matrix
    vec3 T = normalize(vec3(model * vec4(vTangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normalize(vNormal), 0.0)));

    T = normalize(T - dot(T, N) * N); // re-orthogonalize T with respect to N
    vec3 B = cross(N, T); // then retrieve perpendicular vector B with the cross product of T and N

    if (dot(cross(N, T), B) < 0.0)
        T = T * -1.0;

    TBN = mat3(view) * mat3(T, B, N);
}