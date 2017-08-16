#version 330 core

struct DirectionalLight {
  vec3 direction;
  vec3 color;
};

uniform sampler2DShadow shadowTextures[4];
uniform sampler2D tex4;
uniform float cascadeDistances[4];
uniform vec3 cameraPos;
uniform DirectionalLight sun;

in vec3 fragNormal;
in vec3 fragColor;
in vec2 uv;
in vec4 fragLightSpace[4];
in vec3 fragPosition;
in float AO;

out vec4 color;

const float b = 0.004;

vec2 poissonDisk[16] = vec2[](
   vec2( -0.94201624, -0.39906216 ),
   vec2( 0.94558609, -0.76890725 ),
   vec2( -0.094184101, -0.92938870 ),
   vec2( 0.34495938, 0.29387760 ),
   vec2( -0.91588581, 0.45771432 ),
   vec2( -0.81544232, -0.87912464 ),
   vec2( -0.38277543, 0.27676845 ),
   vec2( 0.97484398, 0.75648379 ),
   vec2( 0.44323325, -0.97511554 ),
   vec2( 0.53742981, -0.47373420 ),
   vec2( -0.26496911, -0.41893023 ),
   vec2( 0.79197514, 0.19090188 ),
   vec2( -0.24188840, 0.99706507 ),
   vec2( -0.81409955, 0.91437590 ),
   vec2( 0.19984126, 0.78641367 ),
   vec2( 0.14383161, -0.14100790 )
);

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

float ShadowCalculationPCF(vec4 fragPosLightSpace, sampler2DShadow shadowMap)
{
    float visibility = 1.0;
	float bias = 0.0008;

	for (int i=0;i<8;i++){
		int index = i;
		visibility -= (1.0/8.0)*(1.0-texture( shadowMap, vec3(fragPosLightSpace.xy + poissonDisk[index]/1000.0, (fragPosLightSpace.z)/fragPosLightSpace.w-bias) ));
	}
	return clamp(visibility, 0, 1);
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
	float shadow0 = mix(1.0, ShadowCalculationPCF(fragLightSpace[0], shadowTextures[0]), distance < cascadeDistances[0]);
	float shadow1 = mix(1.0, ShadowCalculation(fragLightSpace[1], shadowTextures[1]), distance > cascadeDistances[0] && distance < cascadeDistances[1]);
	float shadow2 = mix(1.0, ShadowCalculation(fragLightSpace[2], shadowTextures[2]), distance > cascadeDistances[1] && distance < cascadeDistances[2]);
	float shadow3 = mix(1.0, ShadowCalculation(fragLightSpace[3], shadowTextures[3]), distance > cascadeDistances[2] && distance < cascadeDistances[3]);
	float shadow = shadow0 * shadow1 * shadow2 * shadow3;

	vec4 blockTex = texture(tex4, uv);

	vec3 diffuse = max(dot(fragNormal, -sun.direction), 0.0) * sun.color * shadow;

	vec3 viewDir = normalize(cameraPos - fragPosition);
    vec3 reflectDir = reflect(-sun.direction, fragNormal);

    vec3 specular = 0.1f * pow(max(dot(viewDir, reflectDir), 0.0), 8) * sun.color * shadow;

	vec3 diffuse2 = max(dot(fragNormal, normalize(vec3(-1.1, -0.9, -1.0))), 0.0) * vec3(1.0, 1.0, 1.0);
	diffuse2 += max(dot(fragNormal, normalize(vec3(1.1, 0.9, 1.0))), 0.0) * vec3(1.0, 1.0, 1.0);

    vec3 finalColor = AO * blockTex.rgb * (diffuse + diffuse2 + specular);
    finalColor = applyFog(finalColor, distance, normalize(cameraPos - fragPosition), -sun.direction);

	if (blockTex.a > 0) {
		color = vec4(finalColor, 1.0);
	}
	else {
		discard;
	}
}
