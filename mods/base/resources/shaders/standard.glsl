#pragma BEGINVERTEX

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

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gRoughnessMetallic;

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoord;
in mat3 TBN;
in vec3 debug;

uniform sampler2D albedoTex;
uniform bool albedoTextured;
uniform vec3 albedoColor;

uniform sampler2D roughnessTex;
uniform bool roughnessTextured;
uniform float roughnessColor;

uniform sampler2D metallicTex;
uniform bool metallicTextured;
uniform float metallicColor;

uniform sampler2D normalTex;
uniform bool normalTextured;

uniform sampler2D aoTex;
uniform bool aoTextured;

uniform sampler2D emissionTex;
uniform bool emissionTextured;
uniform float emission;

uniform bool invertRoughness;
uniform bool roughnessMetallicAlpha;

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPosition = fPosition;

	vec4 albedo;
    float roughness;
    float metallic;
    vec3 normal;
    float ao;
    float emis;

    if (albedoTextured) {
    	vec4 texel = texture(albedoTex, fTexCoord);
        albedo = vec4(pow(texel.rgb, vec3(2.2)), texel.a);
    }
    else {
        albedo = vec4(albedoColor, 1.0);
    }

    if (roughnessTextured) {
        roughness = texture(roughnessTex, fTexCoord).r;
    }
    else {
        roughness = roughnessColor;
    }

    if (metallicTextured) {
        metallic = texture(metallicTex, fTexCoord).r;

        if (roughnessMetallicAlpha) {
            roughness = texture(metallicTex, fTexCoord).a;
        }
    }
    else {
        metallic = metallicColor;
    }

    if (normalTextured) {
    	normal = texture(normalTex, fTexCoord).rgb;
    	normal = normalize(normal * 2.0 - 1.0);
    	normal = normalize(TBN * normal);
    }
    else {
        normal = normalize(fNormal);
    }

    if (aoTextured) {
        ao = texture(aoTex, fTexCoord).r;
    }
    else {
        ao = 1.0;
    }

    if (emissionTextured) {
        emis = length(texture(emissionTex, fTexCoord).rgb) * 5;
    }
    else {
        emis = emission * 10;
    }

    if (invertRoughness) {
        roughness = 1.0-roughness;
    }

    if (albedo.a > 0.5) {
        gNormal = normalize(normal);
        gAlbedo = albedo;
        gRoughnessMetallic = vec4(roughness, metallic, ao, emis);
        //gAlbedo = vec4(debug, 1.0);
    }
    else {
        discard;
    }
}

#pragma ENDFRAGMENT