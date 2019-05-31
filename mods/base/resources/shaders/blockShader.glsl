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

void main()
{
    vec4 viewPos = view * model * vec4(position, 1.0);
    fragPos = viewPos.xyz;
    ao = pow(aos, 3);

    gl_Position = projection * viewPos;

    // mat3 normalMatrix = transpose(inverse(mat3()));
    fragNormal = mat3(view * model) * normalLookup[normalIndex];
    uv = uvs;


    fragColor = vec3(1.0);
}

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

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

#pragma ENDFRAGMENT