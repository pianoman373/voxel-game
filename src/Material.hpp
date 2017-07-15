#pragma once

#include <map>
#include <string>

#include "Math.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

struct UniformTexture {
    Texture tex;
    unsigned int unit;
};

/**
 * Stores a list of uniforms to pass on to a shader.
 */
class Material {
private:
    Shader shader;

    std::map<std::string, UniformTexture> textures;

    std::map<std::string, vec3> vec3Uniforms;

    std::map<std::string, float> floatUniforms;

    std::map<std::string, bool> boolUniforms;

public:
    Material();

    void setUniformTexture(std::string name, Texture value, unsigned int unit = 0);

    void setUniformVec3(std::string name, vec3 value);

    void setUniformFloat(std::string name, float value);

    void setUniformBool(std::string name, bool value);

    vec3 *getUniformVec3(std::string name);

    float *getUniformFloat(std::string name);

    bool *getUniformBool(std::string name);

    void setShader(Shader shader);

    Shader getShader();

    std::map<std::string, UniformTexture> *getTextureUniforms();

    std::map<std::string, vec3> *getVec3Uniforms();

    std::map<std::string, float> *getFloatUniforms();

    std::map<std::string, bool> *getBoolUniforms();

    /**
     * Binds all specified uniforms to this material's specified shader (also binds specified textures).
     * Assumes currently bound shader is the same as the material's.
     */
    void bindUniforms();
};
