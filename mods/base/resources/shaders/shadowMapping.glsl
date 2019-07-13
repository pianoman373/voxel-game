#ifndef SHADOWMAPPING_GLSL
#define SHADOWMAPPING_GLSL

const mat4 biasMatrix = mat4(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
);

const vec2 poissonDisk[16] = vec2[](
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

float ShadowCalculationPCF(float bias, vec4 fragPosLightSpace, sampler2D shadowMap) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;


    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth- bias > closestDepth  ? 1.0 : 0.0;

    float distance = clamp((currentDepth-closestDepth)*250.0, 0.0, 2.0);

    for (int i=0;i<8;i++){
        int index = i;

        float closestDepthN = texture(shadowMap, projCoords.xy + (poissonDisk[index]/1000.0)*distance).r;
        float currentDepthN = projCoords.z;

        shadow += currentDepthN- bias > closestDepthN  ? 1.0 : 0.0;
    }
    shadow /= 9.0;


    return 1.0-shadow;
}

float ShadowCalculation(float bias, vec4 fragPosLightSpace, sampler2D shadowMap) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth- bias > closestDepth  ? 1.0 : 0.0;

    return 1.0-shadow;
}

float ShadowCalculation(float bias, vec4 fragPosLightSpace, vec2 offset, sampler2D shadowMap) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy+offset).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth- bias > closestDepth  ? 1.0 : 0.0;

    return 1.0-shadow;
}

// all credit for these functions goes to http://codeflow.org/entries/2013/feb/15/soft-shadow-mapping/
float texture2DCompare(sampler2D depths, vec2 uv, float compare) {
    float depth = texture2D(depths, uv).r;
    return step(compare, depth);
}

float texture2DShadowLerp(sampler2D depths, vec2 size, vec2 uv, float compare) {
    vec2 texelSize = vec2(1.0)/size;
    vec2 f = fract(uv*size+0.5);
    vec2 centroidUV = floor(uv*size+0.5)/size;

    float lb = texture2DCompare(depths, centroidUV+texelSize*vec2(0.0, 0.0), compare);
    float lt = texture2DCompare(depths, centroidUV+texelSize*vec2(0.0, 1.0), compare);
    float rb = texture2DCompare(depths, centroidUV+texelSize*vec2(1.0, 0.0), compare);
    float rt = texture2DCompare(depths, centroidUV+texelSize*vec2(1.0, 1.0), compare);
    float a = mix(lb, lt, f.y);
    float b = mix(rb, rt, f.y);
    float c = mix(a, b, f.x);
    return c;
}

//slightly modified to use poisson sampling
float PCF(sampler2D depths, vec2 size, vec2 uv, float compare, float shadowRadius){
    float result = 0.0;
    
        for(int index=0; index < 8; index++){
            vec2 off = poissonDisk[index]/shadowRadius*0.02;
            result += texture2DShadowLerp(depths, size, uv+off, compare);
        }
    
    return result/9.0;
}

float ShadowCalculationLinear(float bias, vec4 fragPosLightSpace, sampler2D shadowMap, float shadowRadius) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)

    return texture2DShadowLerp(shadowMap,textureSize(shadowMap, 0), projCoords.xy, projCoords.z-bias);
}


#endif