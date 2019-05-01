#include "rendering/Shader.hpp"

#include <glad/glad.h>

#include <string>
#include <regex>

const std::string lighting_glsl = R"(
    #ifndef LIGHTING_GLSL
#define LIGHTING_GLSL
struct PointLight {
vec3 position;
vec3 color;
float radius;
};
struct DirectionalLight {
vec3 direction;
vec3 color;
};

const float PI = 3.14159265359;

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
float a      = roughness*roughness;
float a2     = a*a;
float NdotH  = max(dot(N, H), 0.0);
float NdotH2 = NdotH*NdotH;
float nom   = a2;
float denom = (NdotH2 * (a2 - 1.0) + 1.0);
denom = 3.141592 * denom * denom;
return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
float r = (roughness + 1.0);
float k = (r*r) / 8.0;
float nom   = NdotV;
float denom = NdotV * (1.0 - k) + k;
return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
float NdotV = max(dot(N, V), 0.0);
float NdotL = max(dot(N, L), 0.0);
float ggx2  = GeometrySchlickGGX(NdotV, roughness);
float ggx1  = GeometrySchlickGGX(NdotL, roughness);
return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
float RadicalInverse_VdC(uint bits)
{
 bits = (bits << 16u) | (bits >> 16u);
 bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
 bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
 bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
 bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
 return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}
// ----------------------------------------------------------------------------
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
float a = roughness*roughness;
float phi = 2.0 * 3.141592 * Xi.x;
float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
// from spherical coordinates to cartesian coordinates - halfway vector
vec3 H;
H.x = cos(phi) * sinTheta;
H.y = sin(phi) * sinTheta;
H.z = cosTheta;
// from tangent-space H vector to world-space sample vector
vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
vec3 tangent   = normalize(cross(up, N));
vec3 bitangent = cross(N, tangent);
vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
return normalize(sampleVec);
}
// ----------------------------------------------------------------------------
vec2 IntegrateBRDF(float NdotV, float roughness)
{
vec3 V;
V.x = sqrt(1.0 - NdotV*NdotV);
V.y = 0.0;
V.z = NdotV;
float A = 0.0;
float B = 0.0;
vec3 N = vec3(0.0, 0.0, 1.0);
const uint SAMPLE_COUNT = 1024u;
for(uint i = 0u; i < SAMPLE_COUNT; ++i)
{
    // generates a sample vector that's biased towards the
    // preferred alignment direction (importance sampling).
    vec2 Xi = Hammersley(i, SAMPLE_COUNT);
    vec3 H = ImportanceSampleGGX(Xi, N, roughness);
    vec3 L = normalize(2.0 * dot(V, H) * H - V);
    float NdotL = max(L.z, 0.0);
    float NdotH = max(H.z, 0.0);
    float VdotH = max(dot(V, H), 0.0);
    if(NdotL > 0.0)
    {
        float G = GeometrySmith(N, V, L, roughness);
        float G_Vis = (G * VdotH) / (NdotH * NdotV);
        float Fc = pow(1.0 - VdotH, 5.0);
        A += (1.0 - Fc) * G_Vis;
        B += Fc * G_Vis;
    }
}
A /= float(SAMPLE_COUNT);
B /= float(SAMPLE_COUNT);
return vec2(A, B);
}
// ----------------------------------------------------------------------------
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
// ----------------------------------------------------------------------------
float ShadowCalculationPCF(float bias, vec4 fragPosLightSpace, sampler2D shadowMap)
{
//    float visibility = 1.0;
//    for (int i=0;i<8;i++){
//        int index = i;
//        visibility -= (1.0/8.0)*(1.0-texture( shadowMap, vec3(fragPosLightSpace.xy + poissonDisk[index]/1000.0, (fragPosLightSpace.z)/fragPosLightSpace.w-bias) ));
//    }
//    //return clamp(visibility, 0, 1);
//    return distance;

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
// ----------------------------------------------------------------------------
float ShadowCalculation(float bias, vec4 fragPosLightSpace, sampler2D shadowMap)
{
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

float ShadowCalculation(float bias, vec4 fragPosLightSpace, vec2 offset, sampler2D shadowMap)
{
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
// ---------------------------------------------------------------------------------------------------
float texture2DCompare(sampler2D depths, vec2 uv, float compare){
    float depth = texture2D(depths, uv).r;
    return step(compare, depth);
}

float texture2DShadowLerp(sampler2D depths, vec2 size, vec2 uv, float compare){
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
// ---------------------------------------------------------------------------------------------------

float ShadowCalculationLinear(float bias, vec4 fragPosLightSpace, sampler2D shadowMap, float shadowRadius) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)

    return PCF(shadowMap,textureSize(shadowMap, 0), projCoords.xy, projCoords.z-bias, shadowRadius);
}


#endif
)";

std::string str_replace( std::string const & in, std::string const & from, std::string const & to )
{
    return std::regex_replace( in, std::regex(from), to );
}

void loadLibraries(std::string &in) {
    //allow 5 levels of recursion
    for (int i = 0; i < 5; i++) {
        in = str_replace(in, "#include <lighting>", lighting_glsl);
    }
}


Shader::Shader() {

}

void Shader::load(std::string vertex, std::string fragment, std::string geometry) {
    loadLibraries(vertex);
    loadLibraries(fragment);

    const char* vShaderCode = vertex.c_str();
    const char* fShaderCode = fragment.c_str();
    const char* fGeometryCode = geometry.c_str();

    //shaders
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl << vertex << std::endl;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl << fragment << std::endl;
    }

    unsigned int geometryShader = 0;

    if (!geometry.empty()) {
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &fGeometryCode, NULL);
        glCompileShader(geometryShader);
    }

    this->id = glCreateProgram();

    glAttachShader(this->id, vertexShader);
    glAttachShader(this->id, fragmentShader);
    if (!geometry.empty()) {
        glAttachShader(this->id, geometryShader);
    }

    glLinkProgram(this->id);

    glGetProgramiv(this->id, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(this->id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::loadPostProcessing(std::string shader) {
    std::string fullShader = R"(
#version 330 core
layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outColor1;
in vec2 fTexCoord;

)" + shader + R"(
void main()
{
    outColor = vec4(postProcess(fTexCoord), 1.0);
}
)";
    load(R"(
#version 330 core
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;

out vec2 fTexCoord;

void main()
{
    fTexCoord = vTexCoord;
    gl_Position = vec4(vPosition, 1.0);
}
)", fullShader);

}



void Shader::bind() const {
    glUseProgram(this->id);
}

void Shader::uniformMat4(const std::string &location, const mat4 &mat) const {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());

    float matrixArray[] = {
        mat.m00, mat.m10, mat.m20, mat.m30,
        mat.m01, mat.m11, mat.m21, mat.m31,
        mat.m02, mat.m12, mat.m22, mat.m32,
        mat.m03, mat.m13, mat.m23, mat.m33
    };
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, matrixArray);
}

void Shader::uniformVec3(const std::string &location, const vec3 &vec) const {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());
    glUniform3f(transformLoc, vec.x, vec.y, vec.z);
}

void Shader::uniformVec4(const std::string &location, const vec4 &vec) const {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());
    glUniform4f(transformLoc, vec.x, vec.y, vec.z, vec.w);
}

void Shader::uniformInt(const std::string &location, int value) const {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());
    glUniform1i(transformLoc,value);
}

void Shader::uniformFloat(const std::string &location, float value) const {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());
    glUniform1f(transformLoc,value);
}

void Shader::uniformBool(const std::string &location, bool value) const {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());
    glUniform1i(transformLoc,value);
}
