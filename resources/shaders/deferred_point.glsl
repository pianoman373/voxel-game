#include <lighting>
#define MAX_POINT_LIGHTS 100
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gRoughnessMetallic;

uniform int pointLightCount;
uniform PointLight[MAX_POINT_LIGHTS] pointLights;

vec3 postProcess(vec2 texCoord) {
    // retrieve data from gbuffer
    vec3 fragPos = texture(gPosition, texCoord).rgb;
    vec3 normal = normalize(texture(gNormal, texCoord).rgb);
    vec3 albedo = texture(gAlbedo, texCoord).rgb;
	vec4 RoughnessMetallic = texture(gRoughnessMetallic, texCoord);
	float roughness = RoughnessMetallic.r;
	float metallic = clamp(RoughnessMetallic.g, 0.0, 1.0);
    float emission = RoughnessMetallic.a;
    float ao = RoughnessMetallic.b;


    vec3 N = normal;
	vec3 V = normalize(-fragPos);
	vec3 R = reflect(-V, N);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    // reflectance equation
    vec3 Lo = vec3(0.0);
    
    for (int i = 0; i < pointLightCount; i++) {
		if (i < MAX_POINT_LIGHTS) {
			PointLight light = pointLights[i];
            // calculate per-light radiance
            vec3 L = normalize(light.position - fragPos);
            vec3 H = normalize(V + L);
            float distance    = length(light.position - fragPos);
            float attenuation = pow(clamp(1.0 - pow(distance / light.radius, 1.0), 0.0, 1.0), 2.0) / (distance * distance + 1.0);
            vec3 radiance     = light.color * 100.0 * attenuation;
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
            if (distance < light.radius) {
                Lo += (kD * albedo / PI + specular) * radiance * NdotL;
            }
		}
	}

    //vec3 light = Albedo;
	if (length(fragPos) > 0.0) {
		return Lo;
	}
	else {
		discard;
	}
}