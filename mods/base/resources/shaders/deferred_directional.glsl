#pragma BEGINVERTEX

#include "postProcessingVertex.glsl"

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

layout (location = 0) out vec4 outColor;

in vec2 fTexCoord;

#include "lighting.glsl"

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gRoughnessMetallic;

uniform mat4 view;

uniform DirectionalLight sun;

vec3 lighting(vec3 fragPos, vec3 albedo, vec3 normal, float roughness, float metallic) {
	vec3 N = normal;
	vec3 V = normalize(-fragPos);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    // reflectance equation
    vec3 Lo = vec3(0.0);
	
	//directional light
	{
		DirectionalLight light = sun;
		// calculate per-light radiance
        vec3 L = normalize(-sun.direction);
        vec3 H = normalize(V + L);
        vec3 radiance     = light.color;
        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlickRoughness(clamp(dot(H, V), 0.0, 1.0), F0, roughness);
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        vec3 nominator    = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular     = nominator / denominator;
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}
    
    return max(Lo, vec3(0));
}

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

    vec3 light = lighting(FragPos, Albedo, Normal, Roughness, Metallic);
    outColor = vec4(light * AO, 1.0);
}

#pragma ENDFRAGMENT