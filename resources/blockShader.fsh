#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gRoughnessMetallic;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 uv;
in vec3 fragColor;
in mat3 TBN;

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

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = fragPos;

  vec4 albedo;
    float roughness;
    float metallic;
    vec3 normal;

    if (albedoTextured) {
      vec4 texel = texture(albedoTex, uv);
        albedo = vec4(pow(texel.rgb, vec3(2.2)), texel.a);

      if (texel.a < 0.1) {
        discard;
      }
    }
    else {
        albedo = vec4(albedoColor, 1.0);
    }

    if (roughnessTextured) {
        roughness = texture(roughnessTex, uv).r;
    }
    else {
        roughness = roughnessColor;
    }

    if (metallicTextured) {
        metallic = texture(metallicTex, uv).r;
    }
    else {
        metallic = metallicColor;
    }

    if (normalTextured) {
      normal = texture(normalTex, uv).rgb;
      normal = normalize(normal * 2.0 - 1.0);
      normal = normalize(TBN * normal);
    }
    else {
        normal = fragNormal;
    }

  gNormal = normalize(normal);
  gAlbedo = vec4(albedo.rgb * length(normal) * (fragColor*0.9 + 0.1), 1.0);
  gRoughnessMetallic = vec4(roughness, metallic, 1.0, 0.0);
}  