#version 330 core

uniform sampler2DShadow shadowTextures[4];
uniform float cascadeDistances[4];
uniform vec3 cameraPos;
uniform vec3 sunDirection;
uniform vec3 sunColor;

in vec3 fragNormal;
in vec4 fragColor;
in vec2 uv;
in vec4 fragLightSpace[4];
in vec3 fragPosition;
in float AO;

out vec4 color;

const float b = 0.004;

vec3 applyFog( in vec3  rgb,      // original color of the pixel
               in float distance, // camera to point distance
               in vec3  rayDir,   // camera to point vector
               in vec3  sunDir )  // sun light direction
{
    float fogAmount = 1.0 - exp( -distance*b );
    float sunAmount = max( dot( rayDir, sunDir ), 0.0 );
    vec3  fogColor  = mix( vec3(0.5,0.6,1.0), // bluish
                           vec3(1.2,0.9,0.7), // yellowish
                           pow(sunAmount,2.0) );
    return mix( rgb, fogColor, fogAmount );
}

float ShadowCalculation(vec4 fragPosLightSpace, sampler2DShadow shadowMap)
{
    float visibility = 1.0;
	float bias = 0.0003;

	visibility -= (1.0-texture( shadowMap, vec3(fragPosLightSpace.xy, (fragPosLightSpace.z)/fragPosLightSpace.w-bias) ));

	return clamp(visibility, 0, 1);
}

void main() {
	float distance = length(cameraPos - fragPosition);
	float shadow1 = mix(1.0, ShadowCalculation(fragLightSpace[1], shadowTextures[1]), distance < cascadeDistances[1]);
	float shadow2 = mix(1.0, ShadowCalculation(fragLightSpace[2], shadowTextures[2]), distance > cascadeDistances[1] && distance < cascadeDistances[2]);
	float shadow3 = mix(1.0, ShadowCalculation(fragLightSpace[3], shadowTextures[3]), distance > cascadeDistances[2] && distance < cascadeDistances[3]);
	float shadow = shadow1 * shadow2 * shadow3;

	vec3 diffuse = max(dot(fragNormal, sunDirection), 0.0) * sunColor * shadow;

	vec3 diffuse2 = max(dot(fragNormal, normalize(vec3(-1.1, -0.9, -1.0))), 0.0) * vec3(1.0, 1.0, 1.0);
	diffuse2 += max(dot(fragNormal, normalize(vec3(1.1, 0.9, 1.0))), 0.0) * vec3(1.0, 1.0, 1.0);

	vec3 finalColor = AO * fragColor.rgb * (diffuse + diffuse2);
	finalColor = applyFog(finalColor, distance, normalize(cameraPos - fragPosition), -sunDirection);

	color = vec4(finalColor, 1.0);
}
