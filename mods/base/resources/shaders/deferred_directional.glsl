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

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

uniform mat4 view;

uniform DirectionalLight sun;

void main() {
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, fTexCoord).rgb;

    if (length(FragPos) == 0.0) {
        discard;
    }

    vec3 Normal = normalize(texture(gNormal, fTexCoord).rgb);
    vec3 Albedo = texture(gAlbedo, fTexCoord).rgb;
    float AO = texture(gAlbedo, fTexCoord).a;
	vec4 RoughnessMetallic = texture(gRoughnessMetallic, fTexCoord);
	float Roughness = RoughnessMetallic.r;
	float Metallic = clamp(RoughnessMetallic.g, 0.0, 1.0);

    vec3 light = directionalLighting(sun.direction, sun.color, FragPos, Albedo, Normal, Roughness, Metallic);
    outColor = vec4(light * AO, 1.0);
}

#pragma ENDFRAGMENT