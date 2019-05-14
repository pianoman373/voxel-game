#include "rendering/Material.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Resources.hpp"

#include <fstream>



Material::Material() {

}

void Material::setDefaultPBRUniforms() {
    this->shader = Resources::standardShader;

    setUniformBool("albedoTextured", false);
    setUniformVec3("albedoColor", vec3(0.3f));

    setUniformBool("roughnessTextured", false);
    setUniformFloat("roughnessColor", 0.5f);

    setUniformBool("metallicTextured", false);
    setUniformFloat("metallicColor", 0.0f);

    setUniformBool("normalTextured", false);

    setUniformBool("aoTextured", false);

    setUniformBool("emissionTextured", false);
    setUniformFloat("emission", 0.0f);

    setUniformBool("invertRoughness", false);
    setUniformBool("roughnessMetallicAlpha", false);
}

void Material::fromJson(const json &j, const Path &workingDirectory) {
    setDefaultPBRUniforms();
    json uniforms = j["uniforms"];

    int textures = 0;

    if (j["name"].is_string()) {
        name = j["name"];
    }

    for (auto it = uniforms.begin(); it != uniforms.end(); ++it)
    {
        std::string key = it.key();
        json value = it.value();

        if (value.is_boolean()) {
            setUniformBool(key, value.get<bool>());
        }
        if (value.is_string()) {
            if (!value.get<std::string>().empty()) {

                setUniformTexture(key, Resources::getTexture(workingDirectory.appendPath(value.get<std::string>())), textures);

                textures++;
            }
        }
        if (value.is_array()) {
            if (value.size() == 3) {
                setUniformVec3(key, vec3(value[0].get<float>(), value[1].get<float>(), value[2].get<float>()));
            }
        }
        if (value.is_number()) {
            setUniformFloat(key, value.get<float>());
        }
    }
}

json Material::toJson(const Path &workingDirectory) const {
    json ret;
    json uniforms;

    const std::map<std::string, float> &floatUniforms = getFloatUniforms();
    for (auto it = floatUniforms.begin(); it != floatUniforms.end(); ++it)
    {
        //shader.uniformFloat(it.first, it.second);
        uniforms[it->first] = it->second;
    }

    //boolean
    const std::map<std::string, bool> &boolUniforms = getBoolUniforms();
    for (auto it = boolUniforms.begin(); it != boolUniforms.end(); ++it)
    {
        //shader.uniformBool(it.first, it.second);
        uniforms[it->first] = it->second;
    }

    //vec3
    const std::map<std::string, vec3> &vec3Uniforms = getVec3Uniforms();
    for (auto it = vec3Uniforms.begin(); it != vec3Uniforms.end(); ++it)
    {
        //shader.uniformVec3(it.first, it.second);
       uniforms[it->first] = {it->second.x, it->second.y, it->second.z};
    }

    //textures
    const std::map<std::string, UniformTexture> &textures = getTextureUniforms();
    for (auto it = textures.begin(); it != textures.end(); ++it)
    {
//        UniformTexture uniform = it.second;
//        uniform.tex.bind(uniform.unit);
//        shader.uniformInt(it.first, uniform.unit);

        // TODO: Find file path from Resources map
        //uniforms[it->first] = Path(it->second.tex.getFilepath()).relativeTo(workingDirectory);


//        std::cout << "working directory: " << workingDirectory << std::endl;
//        std::cout << "raw path: " << it->second.tex.getFilepath() << std::endl;
//        std::cout << "final path: " << Path::getRelativePath(workingDirectory, it->second.tex.getFilepath()) << std::endl;
    }

    ret["uniforms"] = uniforms;
    ret["name"] = name;

    return ret;
}

//<-----===== non normal =====----->//

void Material::setPBRUniforms(const vec3 &albedo, float roughness, float metallic) {
    setDefaultPBRUniforms();
    setUniformBool("albedoTextured", false);
    setUniformVec3("albedoColor", albedo);
    setUniformBool("roughnessTextured", false);
    setUniformFloat("roughnessColor", roughness);
    setUniformBool("metallicTextured", false);
    setUniformFloat("metallicColor", metallic);
    setUniformBool("normalTextured", false);
}

void Material::setPBRUniforms(const Texture &albedo, float roughness, float metallic) {
    setDefaultPBRUniforms();
    setUniformBool("albedoTextured", true);
    setUniformTexture("albedoTex", albedo, 0);
    setUniformBool("roughnessTextured", false);
    setUniformFloat("roughnessColor", roughness);
    setUniformBool("metallicTextured", false);
    setUniformFloat("metallicColor", metallic);
    setUniformBool("normalTextured", false);
}

void Material::setPBRUniforms(const Texture &albedo, const Texture &roughness, float metallic) {
    setDefaultPBRUniforms();
    setUniformBool("albedoTextured", true);
    setUniformTexture("albedoTex", albedo, 0);
    setUniformBool("roughnessTextured", true);
    setUniformTexture("roughnessTex", roughness, 1);
    setUniformBool("metallicTextured", false);
    setUniformFloat("metallicColor", metallic);
    setUniformBool("normalTextured", false);
}

void Material::setPBRUniforms(const Texture &albedo, float roughness, const Texture &metallic) {
    setDefaultPBRUniforms();
    setUniformBool("albedoTextured", true);
    setUniformTexture("albedoTex", albedo, 0);
    setUniformBool("roughnessTextured", false);
    setUniformFloat("roughnessColor", roughness);
    setUniformBool("metallicTextured", true);
    setUniformTexture("metallicTex", metallic, 2);
    setUniformBool("normalTextured", false);
}

void Material::setPBRUniforms(const Texture &albedo, const Texture &roughness, const Texture &metallic) {
    setDefaultPBRUniforms();
    setUniformBool("albedoTextured", true);
    setUniformTexture("albedoTex", albedo, 0);
    setUniformBool("roughnessTextured", true);
    setUniformTexture("roughnessTex", roughness, 1);
    setUniformBool("metallicTextured", true);
    setUniformTexture("metallicTex", metallic, 2);
    setUniformBool("normalTextured", false);
}

void Material::setPBRUniforms(const vec3 &albedo, const Texture &roughness, float metallic) {
    setDefaultPBRUniforms();
    setUniformBool("albedoTextured", false);
    setUniformVec3("albedoColor", albedo);
    setUniformBool("roughnessTextured", true);
    setUniformTexture("roughnessTex", roughness, 1);
    setUniformBool("metallicTextured", false);
    setUniformFloat("metallicColor", metallic);
    setUniformBool("normalTextured", false);
}

void Material::setPBRUniforms(const vec3 &albedo, float roughness, const Texture &metallic) {
    setDefaultPBRUniforms();
    setUniformBool("albedoTextured", false);
    setUniformVec3("albedoColor", albedo);
    setUniformBool("roughnessTextured", false);
    setUniformFloat("roughnessColor", roughness);
    setUniformBool("metallicTextured", true);
    setUniformTexture("metallicTex", metallic, 2);
    setUniformBool("normalTextured", false);
    
}

void Material::setPBRUniforms(const vec3 &albedo, const Texture &roughness, const Texture &metallic) {
    setDefaultPBRUniforms();
    setUniformBool("albedoTextured", false);
    setUniformVec3("albedoColor", albedo);
    setUniformBool("roughnessTextured", true);
    setUniformTexture("roughnessTex", roughness, 1);
    setUniformBool("metallicTextured", true);
    setUniformTexture("metallicTex", metallic, 2);
    setUniformBool("normalTextured", false);
    setUniformFloat("emission", 0.0f);
}

//<-----===== normal =====----->//

void Material::setPBRUniforms(const vec3 &albedo, float roughness, float metallic, const Texture &normal) {
    setDefaultPBRUniforms();
	setUniformBool("albedoTextured", false);
	setUniformVec3("albedoColor", albedo);
	setUniformBool("roughnessTextured", false);
	setUniformFloat("roughnessColor", roughness);
	setUniformBool("metallicTextured", false);
	setUniformFloat("metallicColor", metallic);
	setUniformTexture("normalTex", normal, 3);
	setUniformBool("normalTextured", true);
    setUniformFloat("emission", 0.0f);
}

void Material::setPBRUniforms(const Texture &albedo, float roughness, float metallic, const Texture &normal) {
    setDefaultPBRUniforms();
	setUniformBool("albedoTextured", true);
	setUniformTexture("albedoTex", albedo, 0);
	setUniformBool("roughnessTextured", false);
	setUniformFloat("roughnessColor", roughness);
	setUniformBool("metallicTextured", false);
	setUniformFloat("metallicColor", metallic);
	setUniformTexture("normalTex", normal, 3);
	setUniformBool("normalTextured", true);
    setUniformFloat("emission", 0.0f);
}

void Material::setPBRUniforms(const Texture &albedo, const Texture &roughness, float metallic, const Texture &normal) {
    setDefaultPBRUniforms();
	setUniformBool("albedoTextured", true);
	setUniformTexture("albedoTex", albedo, 0);
	setUniformBool("roughnessTextured", true);
	setUniformTexture("roughnessTex", roughness, 1);
	setUniformBool("metallicTextured", false);
	setUniformFloat("metallicColor", metallic);
	setUniformTexture("normalTex", normal, 3);
	setUniformBool("normalTextured", true);
    setUniformFloat("emission", 0.0f);
}

void Material::setPBRUniforms(const Texture &albedo, float roughness, const Texture &metallic, const Texture &normal) {
    setDefaultPBRUniforms();
	setUniformBool("albedoTextured", true);
	setUniformTexture("albedoTex", albedo, 0);
	setUniformBool("roughnessTextured", false);
	setUniformFloat("roughnessColor", roughness);
	setUniformBool("metallicTextured", true);
	setUniformTexture("metallicTex", metallic, 2);
	setUniformTexture("normalTex", normal, 3);
	setUniformBool("normalTextured", true);
    setUniformFloat("emission", 0.0f);
}

void Material::setPBRUniforms(const Texture &albedo, const Texture &roughness, const Texture &metallic, const Texture &normal) {
    setDefaultPBRUniforms();
	setUniformBool("albedoTextured", true);
	setUniformTexture("albedoTex", albedo, 0);
	setUniformBool("roughnessTextured", true);
	setUniformTexture("roughnessTex", roughness, 1);
	setUniformBool("metallicTextured", true);
	setUniformTexture("metallicTex", metallic, 2);
	setUniformTexture("normalTex", normal, 3);
	setUniformBool("normalTextured", true);
    setUniformFloat("emission", 0.0f);
}

void Material::setPBRUniforms(const vec3 &albedo, const Texture &roughness, float metallic, const Texture &normal) {
    setDefaultPBRUniforms();
	setUniformBool("albedoTextured", false);
	setUniformVec3("albedoColor", albedo);
	setUniformBool("roughnessTextured", true);
	setUniformTexture("roughnessTex", roughness, 1);
	setUniformBool("metallicTextured", false);
	setUniformFloat("metallicColor", metallic);
	setUniformTexture("normalTex", normal, 3);
	setUniformBool("normalTextured", true);
    setUniformFloat("emission", 0.0f);
}

void Material::setPBRUniforms(const vec3 &albedo, float roughness, const Texture &metallic, const Texture &normal) {
    setDefaultPBRUniforms();
	setUniformBool("albedoTextured", false);
	setUniformVec3("albedoColor", albedo);
	setUniformBool("roughnessTextured", false);
	setUniformFloat("roughnessColor", roughness);
	setUniformBool("metallicTextured", true);
	setUniformTexture("metallicTex", metallic, 2);
	setUniformTexture("normalTex", normal, 3);
	setUniformBool("normalTextured", true);
    setUniformFloat("emission", 0.0f);
}

void Material::setPBRUniforms(const vec3 &albedo, const Texture &roughness, const Texture &metallic, const Texture &normal) {
    setDefaultPBRUniforms();
	setUniformBool("albedoTextured", false);
	setUniformVec3("albedoColor", albedo);
	setUniformBool("roughnessTextured", true);
	setUniformTexture("roughnessTex", roughness, 1);
	setUniformBool("metallicTextured", true);
	setUniformTexture("metallicTex", metallic, 2);
	setUniformTexture("normalTex", normal, 3);
	setUniformBool("normalTextured", true);
    setUniformFloat("emission", 0.0f);
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
