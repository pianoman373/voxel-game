#pragma BEGINVERTEX

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoord;
out mat3 TBN;
out vec3 debug;

void main()
{
    vec4 viewPos;

    mat3 normalMatrix = transpose(inverse(mat3(view * model)));

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

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gRoughnessMetallic;

uniform sampler2D tex;

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoord;
in vec3 fragColor;
in float ao;

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPosition = fPosition;

    vec4 albedo = texture2D(tex, fTexCoord);

    float roughness = 0.9;
    float metallic = 0.0;
    vec3 normal = fNormal;

    gNormal = normalize(normal);
    gAlbedo = vec4(albedo.rgb, 1.0);
    gRoughnessMetallic = vec4(roughness, metallic, 1.0, 0.0);
}

#pragma ENDFRAGMENT