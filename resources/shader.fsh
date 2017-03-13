#version 330 core

uniform sampler2D tex;

in vec3 fragNormal;
in vec3 fragColor;
in vec2 uv;
in vec4 fragLightSpace;

out vec4 color;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(tex, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
    float bias = 0.0005;
	float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  

    return 1.0 - shadow;
} 

void main() {
	vec3 diffuse = max(dot(fragNormal, normalize(vec3(0.9, 1.1, 1.0))), 0.0) * vec3(1.0, 0.9, 0.9) * ShadowCalculation(fragLightSpace);
	vec3 diffuse2 = max(dot(fragNormal, normalize(vec3(-1.1, -0.9, -1.0))), 0.0) * vec3(1.0, 1.0, 1.0);
	diffuse2 += max(dot(fragNormal, normalize(vec3(1.1, 0.9, 1.0))), 0.0) * vec3(1.0, 1.0, 1.0);

	vec3 blockColor = vec3(0.41, 0.50, 0.31); //vec3(0.63, 0.83, 1.0);

	color = vec4(blockColor * fragColor * (diffuse + diffuse2), 1.0);
}