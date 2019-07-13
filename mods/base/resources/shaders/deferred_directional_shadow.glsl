#pragma BEGINVERTEX

#include "postProcessingVertex.glsl"

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

layout (location = 0) out vec4 outColor;

in vec2 fTexCoord;

#include "PBR.glsl"
#include "shadowMapping.glsl"

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gRoughnessMetallic;

uniform sampler2D shadowTextures[8];
uniform float shadowDistances[8];
uniform mat4 lightSpaceMatrix[8];

uniform int numCascades;

uniform mat4 view;

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

uniform DirectionalLight sun;

const vec3 cascadeColors[6] = vec3[6](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 1.0, 0.0),
    vec3(0.0, 1.0, 1.0),
    vec3(1.0, 0.0, 1.0)
);

void main() {
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, fTexCoord).rgb;
    vec3 Normal = normalize(texture(gNormal, fTexCoord).rgb);
    vec3 Albedo = texture(gAlbedo, fTexCoord).rgb;
	vec4 RoughnessMetallic = texture(gRoughnessMetallic, fTexCoord);
	float Roughness = RoughnessMetallic.r;
	float Metallic = clamp(RoughnessMetallic.g, 0.0, 1.0);
    float AO = texture(gAlbedo, fTexCoord).a;


    //CSM
	vec4 fragLightSpace0 = lightSpaceMatrix[0] * vec4(FragPos, 1.0);
    vec4 fragLightSpace1 = lightSpaceMatrix[1] * vec4(FragPos, 1.0);
    vec4 fragLightSpace2 = lightSpaceMatrix[2] * vec4(FragPos, 1.0);
    vec4 fragLightSpace3 = lightSpaceMatrix[3] * vec4(FragPos, 1.0);

    float shadow = 1.0;

    float distance = length(FragPos);

    vec3 shadowColor;

    for (int i = 0; i < numCascades; i++) {
        if (distance < shadowDistances[i]) {
            vec4 fragLightSpace = lightSpaceMatrix[i] * vec4(FragPos, 1.0);
            float bias = 0.0003;

            shadow = ShadowCalculationLinear(bias, fragLightSpace, shadowTextures[i], shadowDistances[i]);
            shadowColor = cascadeColors[i];

            break;
        }
    }

    shadow = mix(shadow, 1.0, smoothstep(shadowDistances[numCascades-1]*0.6, shadowDistances[numCascades-1], distance));

    vec3 light = directionalLighting(sun.direction, sun.color, FragPos, Albedo, Normal, Roughness, Metallic) * shadow;
    //vec3 light = Albedo;
	if (length(FragPos) > 0.0) {
		outColor = vec4(light * AO, 1.0);
	}
	else {
		discard;
	}
}

#pragma ENDFRAGMENT