#include "rendering/Material.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Resources.hpp"

#include <fstream>



Material::Material() {

}

void Material::setUniformTexture(const std::string &name, const Texture &value, unsigned int unit) {
    textures[name].tex = value;
    textures[name].unit = unit;
}

void Material::setUniformCubemap(const std::string &name, const Cubemap &value, unsigned int unit) {
    cubemaps[name].tex = value;
    cubemaps[name].unit = unit;
}

void Material::setUniformTextureArray(const std::string &name, const TextureArray &value, unsigned int unit) {
    textureArrays[name].tex = value;
    textureArrays[name].unit = unit;
}

void Material::setUniformVec3(const std::string &name, const vec3 &value) {
    vec3Uniforms[name] = value;
}

void Material::setUniformFloat(const std::string &name, float value) {
    floatUniforms[name] = value;
}

void Material::setUniformBool(const std::string &name, bool value) {
    boolUniforms[name] = value;
}

vec3 &Material::getUniformVec3(const std::string &name) {
    return vec3Uniforms[name];
}

float &Material::getUniformFloat(const std::string &name) {
    return floatUniforms[name];
}

bool &Material::getUniformBool(const std::string &name) {
    return boolUniforms[name];
}

Texture &Material::getUniformTexture(const std::string &name) {
    return textures[name].tex;
}

void Material::setShader(const Shader &shader) {
    this->shader = shader;
}

const Shader &Material::getShader() const {
    return shader;
}

const std::map<std::string, UniformTexture> &Material::getTextureUniforms() const {
    return textures;
}

const std::map<std::string, UniformCubemap> &Material::getCubemapUniforms() const {
    return cubemaps;
}

const std::map<std::string, UniformTextureArray> &Material::getTextureArrayUniforms() const {
    return textureArrays;
}

const std::map<std::string, vec3> &Material::getVec3Uniforms() const {
    return vec3Uniforms;
}

const std::map<std::string, float> &Material::getFloatUniforms() const {
    return floatUniforms;
}

const std::map<std::string, bool> &Material::getBoolUniforms() const {
    return boolUniforms;
}

void Material::bindUniforms() const {
    //float
    const std::map<std::string, float> &floatUniforms = getFloatUniforms();
    for (auto it = floatUniforms.begin(); it != floatUniforms.end(); ++it)
    {
        shader.uniformFloat(it->first, it->second);
    }

    //boolean
    const std::map<std::string, bool> &boolUniforms = getBoolUniforms();
    for (auto it = boolUniforms.begin(); it != boolUniforms.end(); ++it)
    {
        shader.uniformBool(it->first, it->second);
    }

    //vec3
    const std::map<std::string, vec3> &vec3Uniforms = getVec3Uniforms();
    for (auto it = vec3Uniforms.begin(); it != vec3Uniforms.end(); ++it)
    {
        shader.uniformVec3(it->first, it->second);
    }

    //textures
    const std::map<std::string, UniformTexture> &textures = getTextureUniforms();
    for (auto it = textures.begin(); it != textures.end(); ++it)
    {
        UniformTexture uniform = it->second;
        uniform.tex.bind(uniform.unit);
        shader.uniformInt(it->first, uniform.unit);
    }

    //cubemaps
    const std::map<std::string, UniformCubemap> &cubemaps = getCubemapUniforms();
    for (auto it = cubemaps.begin(); it != cubemaps.end(); ++it)
    {
        UniformCubemap uniform = it->second;
        uniform.tex.bind(uniform.unit);
        shader.uniformInt(it->first, uniform.unit);
    }

    //texture arrays
    const std::map<std::string, UniformTextureArray> &textureArrays = getTextureArrayUniforms();
    for (auto it = textureArrays.begin(); it != textureArrays.end(); ++it)
    {
        UniformTextureArray uniform = it->second;
        uniform.tex.bind(uniform.unit);
        shader.uniformInt(it->first, uniform.unit);
    }
}
