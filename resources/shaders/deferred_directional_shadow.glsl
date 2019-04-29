#include <lighting>

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gRoughnessMetallic;

uniform sampler2D shadowTextures[8];
uniform float shadowDistances[8];
uniform mat4 lightSpaceMatrix[8];

uniform int numCascades;

uniform mat4 view;

uniform DirectionalLight sun;

const vec3 cascadeColors[6] = vec3[6](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 1.0, 0.0),
    vec3(0.0, 1.0, 1.0),
    vec3(1.0, 0.0, 1.0)
);

vec3 lighting(vec3 fragPos, vec3 albedo, vec3 normal, float roughness, float metallic) {
	vec3 N = normal;
	vec3 V = normalize(-fragPos);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    // reflectance equation
    vec3 Lo = vec3(0.0);
    //CSM
	vec4 fragLightSpace0 = lightSpaceMatrix[0] * vec4(fragPos, 1.0);
    vec4 fragLightSpace1 = lightSpaceMatrix[1] * vec4(fragPos, 1.0);
    vec4 fragLightSpace2 = lightSpaceMatrix[2] * vec4(fragPos, 1.0);
    vec4 fragLightSpace3 = lightSpaceMatrix[3] * vec4(fragPos, 1.0);

    float shadow = 1.0;

    float distance = length(fragPos);

    vec3 shadowColor;

    float cosTheta = dot(N, -sun.direction);

    for (int i = 0; i < numCascades; i++) {
        if (distance < shadowDistances[i]) {
            vec4 fragLightSpace = lightSpaceMatrix[i] * vec4(fragPos, 1.0);
            float bias = 0.001*tan(acos(cosTheta));

            shadow = ShadowCalculationLinear(bias, fragLightSpace, shadowTextures[i], shadowDistances[i]);
            shadowColor = cascadeColors[i];

            break;
        }
    }
	
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
        Lo += (kD * albedo / PI + specular) * radiance * NdotL * shadow;
	}
    
    return max(Lo, vec3(0));
}
vec3 postProcess(vec2 texCoord) {
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, texCoord).rgb;
    vec3 Normal = normalize(texture(gNormal, texCoord).rgb);
    vec3 Albedo = texture(gAlbedo, texCoord).rgb;
	vec4 RoughnessMetallic = texture(gRoughnessMetallic, texCoord);
	float Roughness = RoughnessMetallic.r;
	float Metallic = clamp(RoughnessMetallic.g, 0.0, 1.0);

    vec3 light = lighting(FragPos, Albedo, Normal, Roughness, Metallic);
    //vec3 light = Albedo;
	if (length(FragPos) > 0.0) {
		return light;
	}
	else {
		discard;
	}
}