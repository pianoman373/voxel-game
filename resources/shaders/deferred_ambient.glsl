#include <lighting>

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gRoughnessMetallic;
uniform samplerCube irradiance;
uniform samplerCube prefilter;
uniform sampler2D brdf;

uniform mat4 inverseView;

vec3 postProcess(vec2 texCoord) {

    // retrieve data from gbuffer
    vec3 fragPos = texture(gPosition, texCoord).rgb;
    vec3 normal = normalize(texture(gNormal, texCoord).rgb);
    vec3 albedo = texture(gAlbedo, texCoord).rgb;
	vec4 RoughnessMetallic = texture(gRoughnessMetallic, texCoord);
	float roughness = RoughnessMetallic.r;
	float metallic = clamp(RoughnessMetallic.g, 0.0, 1.0);

    vec3 N = normal;
	vec3 V = normalize(-fragPos);

	vec3 R = reflect(-V, N);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 F = fresnelSchlickRoughness(clamp(dot(N, V), 0.0, 1.0), F0, roughness);
	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;

	vec3 irradianceColor = texture(irradiance, (inverseView * vec4(N, 0.0)).xyz).rgb;
	vec3 diffuse = irradianceColor * albedo;

	const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilter, (inverseView * vec4(R, 0.0)).xyz,  roughness * MAX_REFLECTION_LOD).rgb;
	vec2 brdfColor = texture(brdf, vec2(max(dot(N, V), 0.0), roughness)).rg;

	vec3 specular = prefilteredColor * (F * brdfColor.x + brdfColor.y);

	if (length(fragPos) > 0.0) {
		return (kD * diffuse + specular);
	}
	else {
		discard;
	}
}