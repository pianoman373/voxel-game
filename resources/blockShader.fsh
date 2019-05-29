#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gRoughnessMetallic;

in vec3 fragPos;
in vec3 fragNormal;
in vec3 uv;
in vec3 fragColor;
in float ao;

uniform sampler2D tex;
uniform sampler2D emissionTex;
uniform sampler2DArray texArray;

void main()
{    
  // store the fragment position vector in the first gbuffer texture
  gPosition = fragPos;

  vec4 albedo = texture(texArray, uv);

    if (albedo.a < 0.1)
        discard;

  float roughness = 1.0;
  float metallic = 0.0;
  vec3 normal = fragNormal;

  gNormal = normalize(normal);
  gAlbedo = vec4(pow(albedo.rgb, vec3(2.2)), ao);
  gRoughnessMetallic = vec4(roughness, metallic, 1.0, 0.0);
}  