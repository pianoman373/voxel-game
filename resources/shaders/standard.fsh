#version 330 core
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