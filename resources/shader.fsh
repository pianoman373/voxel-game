#version 330 core

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
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

float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
    float bias = 0.0005;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    return 1.0 - shadow;
}

void main() {
  float distance = length(cameraPos - fragPosition);
  float shadow0 = mix(1.0, ShadowCalculation(fragLightSpace[0], tex0), distance < cascadeDistances[0]);
	float shadow1 = mix(1.0, ShadowCalculation(fragLightSpace[1], tex1), distance > cascadeDistances[0] && distance < cascadeDistances[1]);
  float shadow2 = mix(1.0, ShadowCalculation(fragLightSpace[2], tex2), distance > cascadeDistances[1] && distance < cascadeDistances[2]);
  float shadow3 = mix(1.0, ShadowCalculation(fragLightSpace[3], tex3), distance > cascadeDistances[2] && distance < cascadeDistances[3]);
  float shadow = shadow0 * shadow1 * shadow2 * shadow3;

  vec3 blockTex = texture(tex4, uv).rgb;

	vec3 diffuse = max(dot(fragNormal, sunDirection), 0.0) * sunColor * shadow;
	vec3 diffuse2 = max(dot(fragNormal, normalize(vec3(-1.1, -0.9, -1.0))), 0.0) * vec3(1.0, 1.0, 1.0);
	diffuse2 += max(dot(fragNormal, normalize(vec3(1.1, 0.9, 1.0))), 0.0) * vec3(1.0, 1.0, 1.0);

	vec3 blockColor = fragColor.rgb; //vec3(0.63, 0.83, 1.0);
	vec3 fog = clamp((1.0/1000.0) * distance * distance, 0.0, 1.0) * vec3(0.6, 0.8, 1.0);

    vec3 finalColor = blockTex * fragColor.a * (diffuse + diffuse2);
    finalColor = applyFog(finalColor, distance, normalize(cameraPos - fragPosition), -sunDirection);
    //finalColor = applyGroundFog(finalColor, distance, cameraPos, fragPosition - cameraPos);

	color = vec4(finalColor, 1.0);
}
