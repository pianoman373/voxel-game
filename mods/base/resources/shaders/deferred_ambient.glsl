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
uniform samplerCube irradiance;
uniform samplerCube prefilter;
uniform sampler2D brdf;

uniform mat4 inverseView;

void main() {

    // retrieve data from gbuffer
    vec3 fragPos = texture(gPosition, fTexCoord).rgb;
    vec3 normal = normalize(texture(gNormal, fTexCoord).rgb);
    vec3 albedo = texture(gAlbedo, fTexCoord).rgb;
	vec4 RoughnessMetallic = texture(gRoughnessMetallic, fTexCoord);
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
		outColor = vec4(kD * diffuse + specular, 1.0);
	}
	else {
		discard;
	}
}

#pragma ENDFRAGMENT