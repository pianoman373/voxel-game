#pragma BEGINVERTEX

#include "cubemapVertex.glsl"

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

out vec4 outColor;

in vec3 fPosition;

uniform samplerCube environmentMap;

void main()
{
    // the sample direction equals the hemisphere's orientation
    vec3 N = normalize(fPosition);

	vec3 irradiance = vec3(0.0);

	vec3 up    = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, N);
	up         = cross(N, right);

	float sampleDelta = 0.025;
	float nrSamples = 0.0;
	float PI = 3.14159265359;

	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

			irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));

    outColor = vec4(irradiance, 1.0);
}

#pragma ENDFRAGMENT