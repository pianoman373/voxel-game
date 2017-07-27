#include "Material.hpp"

Material::Material() {

}

void Material::setUniformTexture(std::string name, Texture value, unsigned int unit) {
    textures[name].tex = value;
    textures[name].unit = unit;
}

void Material::setUniformVec3(std::string name, vec3 value) {
    vec3Uniforms[name] = value;
}

void Material::setUniformFloat(std::string name, float value) {
    floatUniforms[name] = value;
}

void Material::setUniformBool(std::string name, bool value) {
    boolUniforms[name] = value;
}

vec3 *Material::getUniformVec3(std::string name) {
    return &vec3Uniforms[name];
}

float *Material::getUniformFloat(std::string name) {
    return &floatUniforms[name];
}

bool *Material::getUniformBool(std::string name) {
    return &boolUniforms[name];
}

void Material::setShader(Shader shader) {
    this->shader = shader;
}

Shader Material::getShader() {
    return shader;
}

std::map<std::string, UniformTexture> *Material::getTextureUniforms() {
    return &textures;
}

std::map<std::string, vec3> *Material::getVec3Uniforms() {
    return &vec3Uniforms;
}

std::map<std::string, float> *Material::getFloatUniforms() {
    return &floatUniforms;
}

std::map<std::string, bool> *Material::getBoolUniforms() {
    return &boolUniforms;
}

void Material::bindUniforms() {
    //float
    std::map<std::string, float> *floatUniforms = this->getFloatUniforms();
    for (auto it = floatUniforms->begin(); it != floatUniforms->end(); ++it)
    {
        shader.uniformFloat(it->first, it->second);
    }

    //boolean
    std::map<std::string, bool> *boolUniforms = this->getBoolUniforms();
    for (auto it = boolUniforms->begin(); it != boolUniforms->end(); ++it)
    {
        shader.uniformBool(it->first, it->second);
    }

    //vec3
    std::map<std::string, vec3> *vec3Uniforms = this->getVec3Uniforms();
    for (auto it = vec3Uniforms->begin(); it != vec3Uniforms->end(); ++it)
    {
        shader.uniformVec3(it->first, it->second);
    }

    //textures
    std::map<std::string, UniformTexture> *textures = this->getTextureUniforms();
    for (auto it = textures->begin(); it != textures->end(); ++it)
    {
        UniformTexture uniform = it->second;
        uniform.tex.bind(uniform.unit);
        shader.uniformInt(it->first, uniform.unit);
    }
}
