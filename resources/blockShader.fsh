#version 330 core

uniform sampler2DShadow tex0;
uniform sampler2DShadow tex1;
uniform sampler2DShadow tex2;
uniform sampler2DShadow tex3;
uniform sampler2D tex4;
uniform float cascadeDistances[4];
uniform vec3 cameraPos;
uniform vec3 sunDirection;
uniform vec3 sunColor;

in vec3 fragNormal;
in vec4 fragColor;
in vec2 uv;
in vec4 fragLightSpace[4];
in vec3 fragPosition;

out vec4 color;

const float c = 0.002;
const float b = 0.001;

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
    float fogAmount = 1.0 - exp( -distance*0.005 );
    float sunAmount = max( dot( rayDir, sunDir ), 0.0 );
    vec3  fogColor  = mix( vec3(0.5,0.6,1.0), // bluish
                           vec3(1.2,0.9,0.7), // yellowish
                           pow(sunAmount,2.0) );
    return mix( rgb, fogColor, fogAmount );
}

vec3 applyGroundFog( in vec3  rgb,      // original color of the pixel
               in float distance, // camera to point distance
               in vec3  rayOri,   // camera position
               in vec3  rayDir )  // camera to point vector
{
    float fogAmount = c * exp(-rayOri.y*b) * (1.0-exp( -distance*rayDir.y*b ))/rayDir.y;
    vec3  fogColor  = vec3(0.8,0.8,0.8);
    return mix( rgb, fogColor, clamp(fogAmount, 0.0, 1.0) );
}

float ShadowCalculationPCF(vec4 fragPosLightSpace, sampler2DShadow shadowMap)
{
    float visibility = 1.0;
	float bias = 0.0003;

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
  float shadow0 = mix(1.0, ShadowCalculationPCF(fragLightSpace[0], tex0), distance < cascadeDistances[0]);
  float shadow1 = mix(1.0, ShadowCalculation(fragLightSpace[1], tex1), distance > cascadeDistances[0] && distance < cascadeDistances[1]);
  float shadow2 = mix(1.0, ShadowCalculation(fragLightSpace[2], tex2), distance > cascadeDistances[1] && distance < cascadeDistances[2]);
  float shadow3 = mix(1.0, ShadowCalculation(fragLightSpace[3], tex3), distance > cascadeDistances[2] && distance < cascadeDistances[3]);
  float shadow = shadow0 * shadow1 * shadow2 * shadow3;

  vec4 blockTex = texture(tex4, uv);

	vec3 diffuse = max(dot(fragNormal, sunDirection), 0.0) * sunColor * shadow;

	vec3 viewDir = normalize(cameraPos - fragPosition);
    vec3 reflectDir = reflect(-sunDirection, fragNormal);

    vec3 specular = 0.1f * pow(max(dot(viewDir, reflectDir), 0.0), 8) * sunColor * shadow;

	vec3 diffuse2 = max(dot(fragNormal, normalize(vec3(-1.1, -0.9, -1.0))), 0.0) * vec3(1.0, 1.0, 1.0);
	diffuse2 += max(dot(fragNormal, normalize(vec3(1.1, 0.9, 1.0))), 0.0) * vec3(1.0, 1.0, 1.0);

	vec3 blockColor = fragColor.rgb; //vec3(0.63, 0.83, 1.0);
	vec3 fog = clamp((1.0/1000.0) * distance * distance, 0.0, 1.0) * vec3(0.6, 0.8, 1.0);

    vec3 finalColor = blockTex.rgb * fragColor.a * (diffuse + diffuse2 + specular);
    finalColor = applyFog(finalColor, distance, normalize(cameraPos - fragPosition), -sunDirection);
    //finalColor = applyGroundFog(finalColor, distance, cameraPos, fragPosition - cameraPos);

	if (blockTex.a > 0) {
		color = vec4(finalColor, 1.0);
	}
	else {
		discard;
	}
}
