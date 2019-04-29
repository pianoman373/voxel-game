uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gRoughnessMetallic;
uniform sampler2D deferred;
uniform sampler2D deferredBlur;
uniform samplerCube prefilter;
uniform sampler2D brdf;



uniform mat4 projection;
uniform mat4 view;

#include <lighting>

const float step = 0.1;
const float minRayStep = 0.1;
const float maxSteps = 60;
const int numBinarySearchSteps = 10;

float depthAt(vec2 pos) {
	vec4 texVal = texture(gPosition, pos);

	if (length(texVal.xyz) > 0.0) {
		return texVal.z;
	}
	else {
		return 100000000000.0;
	}

	
}

vec3 binarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth)
{
    float depth;

    vec4 projectedCoord;

    for(int i = 0; i < numBinarySearchSteps; i++)
    {

        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        //depth = (vec4(texture(gPosition, projectedCoord.xy).xyz, 1.0)).z;
        depth = depthAt(projectedCoord.xy);
        dDepth = hitCoord.z - depth;

        dir *= 0.5;
        if(dDepth > 0.0)
            hitCoord += dir;
        else
            hitCoord -= dir;
    }

        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

    return vec3(projectedCoord.xy, depth);
}

vec4 rayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth)
{
    dir *= step;

    float error = 1.0;
    float depth;
    int steps;
    vec4 projectedCoord;

    for(int i = 0; i < maxSteps; i++)
    {
        hitCoord += dir;

        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        //depth = (vec4(texture(gPosition, projectedCoord.xy).xyz, 1.0)).z;
        depth = depthAt(projectedCoord.xy);

        dDepth = hitCoord.z - depth;

        if((dir.z - dDepth) < 1.2)
        {
            if(dDepth <= 0.0)
            {
                error = 0.0;
                vec4 Result;
                Result = vec4(binarySearch(dir, hitCoord, dDepth), 1.0);
                return Result;
            }

        }

        steps++;
    }

    return vec4(projectedCoord.xy, depth, error);
}


vec3 postProcess(vec2 texCoord) {
    mat4 inverseView = inverse(view);

    vec3 viewPos = texture(gPosition, texCoord).rgb;
    vec3 normal = normalize(texture(gNormal, texCoord).rgb);
    float roughness = texture(gRoughnessMetallic, texCoord).r;
    float metallic = texture(gRoughnessMetallic, texCoord).g;
    vec3 albedo = texture(gAlbedo, texCoord).rgb;



    // lighting input
    vec3 N = normalize(normal);
    vec3 V = normalize(viewPos);
    vec3 R = reflect(V, N);

    float frontFacingFactor = smoothstep(0.0, 0.2, dot(V, R));

    vec3 hitPos = viewPos;
    float dDepth;

    vec4 coords = rayMarch(R * max(minRayStep, -viewPos.z), hitPos, dDepth);
    //float error = coords.w;

    vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
    float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

    float roughnessCutoffFactor = smoothstep(0.7, 1.0, 1-roughness);

    float ReflectionMultiplier = screenEdgefactor * frontFacingFactor * roughnessCutoffFactor;

    if (coords.z - viewPos.z > 1.0) {
        ReflectionMultiplier = 0.0;
    }

    if (length(texture(gPosition, coords.xy).rgb) > 0.0) {

    }
    else {
        //ReflectionMultiplier = 0.0;
    }


    vec3 SSR = mix(texture(deferred, coords.xy).rgb, texture(deferredBlur, coords.xy).rgb, roughness) * clamp(ReflectionMultiplier, 0.0, 1.0);


    //specular-----------------------------------
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    vec3 F = fresnelSchlickRoughness(clamp(dot(N, V), 0.0, 1.0), F0, roughness);

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilter, normalize(mat3(inverse(view)) * R),  roughness * MAX_REFLECTION_LOD).rgb;
	vec2 brdfColor = texture(brdf, vec2(max(dot(N, V), 0.0), roughness)).rg;

	vec3 specular = prefilteredColor * (F * brdfColor.x + brdfColor.y);
    //-------------------------------------------

    //return texture(deferred, coords.xy).rgb;


    if (length(texture(gPosition, texCoord).rgb) > 0.0) {
        return texture(deferred, texCoord).rgb + (SSR*F);
    }
    else {
        return texture(deferred, texCoord).rgb;
    }
}
