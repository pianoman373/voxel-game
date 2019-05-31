#pragma BEGINVERTEX

#include "postProcessingVertex.glsl"

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

layout (location = 0) out vec4 outColor;

in vec2 fTexCoord;

#include "lighting.glsl"

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform int kernelSize;

uniform vec3 samples[256];

uniform mat4 projection;

uniform float radius;
uniform float strength;
float bias = 0.025;

// tile noise texture over screen based on screen dimensions divided by noise size
uniform vec3 noiseScale;

void main() {
// get input for SSAO algorithm
    vec3 fragPos = texture(gPosition, fTexCoord).xyz;
    vec3 normal = normalize(texture(gNormal, fTexCoord).rgb);
    vec3 randomVec = normalize(texture(texNoise, fTexCoord * noiseScale.xy).xyz);
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 sample = TBN * samples[i]; // from tangent to view-space
        sample = fragPos + sample * radius;

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(sample, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0


        if (length(texture(gPosition, offset.xy).rgb) > 0.0) {
            // get sample depth
            float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample


            // range check & accumulate
            float rangeCheck = smoothstep(0.0, 1.0, radius / abs((fragPos.z - sampleDepth) * 10));
            occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;
        }
    }
    occlusion = 1.0 - (occlusion*strength / kernelSize);



    if (length(fragPos) > 0.0) {
        outColor = vec4(vec3(occlusion), 1.0);
	}
	else {
		outColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
}

#pragma ENDFRAGMENT