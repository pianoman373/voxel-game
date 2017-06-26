#version 330 core

out vec4 outColor;

in vec3 fragNormal;

void main()
{
	vec3 diffuse = max(dot(fragNormal, normalize(vec3(0.9, 1.7, 2.4))), 0.0) * vec3(1.0, 1.0, 1.0);
	diffuse += max(dot(fragNormal, normalize(vec3(-0.9, -1.7, 1.4))), 0.0) * vec3(0.1, 0.2, 0.3);
	diffuse += vec3(0.2, 0.2, 0.2);

	vec3 albedo = vec3(0.8, 0.8, 0.8);

	outColor = vec4(albedo * diffuse, 1.0f);
}
