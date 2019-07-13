#pragma BEGINVERTEX

#include "postProcessingVertex.glsl"

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

layout (location = 0) out vec4 outColor;

in vec2 fTexCoord;

#include "PBR.glsl"

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gRoughnessMetallic;

uniform mat4 view;


void main() {
    vec3 FragPos = texture(gPosition, fTexCoord).rgb;

    if (length(FragPos) == 0.0) {
        discard;
    }

    // retrieve data from gbuffer
    
    vec3 Normal = normalize(texture(gNormal, fTexCoord).rgb);
    vec3 Albedo = texture(gAlbedo, fTexCoord).rgb;
    float AO = texture(gAlbedo, fTexCoord).a;
	vec4 RoughnessMetallic = texture(gRoughnessMetallic, fTexCoord);
	float Roughness = RoughnessMetallic.r;
	float Metallic = clamp(RoughnessMetallic.g, 0.0, 1.0);

    vec4 dir1 = view * vec4(normalize(vec3(-0.9f, -1.2f, -1.0f)), 0.0);
    vec4 dir2 = view * vec4(normalize(vec3(0.9f, 0.8f, 1.0f)), 0.0);

    vec3 L1 = directionalLighting(dir1.xyz, vec3(0.7f, 0.8f, 1.2f)*3.0f, FragPos, Albedo, Normal, Roughness, Metallic);
    vec3 L2 = directionalLighting(dir2.xyz, vec3(0.5f, 0.6f, 0.9f)*2.5f, FragPos, Albedo, Normal, Roughness, Metallic);
    
    outColor = vec4(vec3(0.01), 1.0);
}

#pragma ENDFRAGMENT