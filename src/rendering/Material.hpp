#pragma once

#include <map>
#include <string>

#include "Math.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Texture.hpp"
#include "util/Path.hpp"

#include "json.hpp"
using nlohmann::json;

struct UniformTexture {
    Texture tex;
    unsigned int unit;
};

struct UniformCubemap {
    Cubemap tex;
    unsigned int unit;
};

enum class BlendType {
    NORMAL,
    ADDITIVE
};

/**
 * Stores a list of uniforms to pass on to a shader. Unlike Textures and Shaders, Materials store all their data in
 * object memory itself. This makes passing Materials by value expensive compared to Shaders and Textures.
 */
class Material {
private:
    Shader shader;

    std::map<std::string, UniformTexture> textures;

    std::map<std::string, UniformCubemap> cubemaps;

    std::map<std::string, vec3> vec3Uniforms;

    std::map<std::string, float> floatUniforms;

    std::map<std::string, bool> boolUniforms;

public:
	std::string name;
    bool deferred = true;
    BlendType blendType = BlendType::NORMAL;

    Material();

    /**
     * Json serialization. Working directory is needed in order to output correct texture paths.
     */
    void fromJson(const json &j, const Path &workingDirectory);

	/**
     * Json serialization. Working directory is needed in order to output correct texture paths.
     */
    json toJson(const Path &workingDirectory) const;

	void setDefaultPBRUniforms();

    //all combinations of textures vs floats etc
    void setPBRUniforms(const vec3 &albedo, float roughness, float metallic);

    void setPBRUniforms(const Texture &albedo, float roughness, float metallic);

    void setPBRUniforms(const Texture &albedo, const Texture &roughness, float metallic);

    void setPBRUniforms(const Texture &albedo, float roughness, const Texture &metallic);

    void setPBRUniforms(const Texture &albedo, const Texture &roughness, const Texture &metallic);

    void setPBRUniforms(const vec3 &albedo, const Texture &roughness, float metallic);

    void setPBRUniforms(const vec3 &albedo, float roughness, const Texture &metallic);

    void setPBRUniforms(const vec3 &albedo, const Texture &roughness, const Texture &metallic);

	//with normal maps
	void setPBRUniforms(const vec3 &albedo, float roughness, float metallic, const Texture &normal);

	void setPBRUniforms(const Texture &albedo, float roughness, float metallic, const Texture &normal);

	void setPBRUniforms(const Texture &albedo, const Texture &roughness, float metallic, const Texture &normal);

	void setPBRUniforms(const Texture &albedo, float roughness, const Texture &metallic, const Texture &normal);

	void setPBRUniforms(const Texture &albedo, const Texture &roughness, const Texture &metallic, const Texture &normal);

	void setPBRUniforms(const vec3 &albedo, const Texture &roughness, float metallic, const Texture &normal);

	void setPBRUniforms(const vec3 &albedo, float roughness, const Texture &metallic, const Texture &normal);

	void setPBRUniforms(const vec3 &albedo, const Texture &roughness, const Texture &metallic, const Texture &normal);



    void setUniformTexture(const std::string &name, const Texture &value, unsigned int unit = 0);

    void setUniformCubemap(const std::string &name, const Cubemap &value, unsigned int unit = 0);

    void setUniformVec3(const std::string &name, const vec3 &value);

    void setUniformFloat(const std::string &name, float value);

    void setUniformBool(const std::string &name, bool value);

    /** Returns a pointer to the queried value. If the value does not exist then it is added to the list of uniforms **/
    vec3 &getUniformVec3(const std::string &name);

	/** Returns a pointer to the queried value. If the value does not exist then it is added to the list of uniforms **/
    float &getUniformFloat(const std::string &name);

	/** Returns a pointer to the queried value. If the value does not exist then it is added to the list of uniforms **/
    bool &getUniformBool(const std::string &name);

	/** Returns a pointer to the queried value. If the value does not exist then it is added to the list of uniforms **/
    Texture &getUniformTexture(const std::string &name);

    void setShader(const Shader &shader);

    const Shader &getShader() const;

    const std::map<std::string, UniformTexture> &getTextureUniforms() const;

    const std::map<std::string, UniformCubemap> &getCubemapUniforms() const;

    const std::map<std::string, vec3> &getVec3Uniforms() const;

    const std::map<std::string, float> &getFloatUniforms() const;

    const std::map<std::string, bool> &getBoolUniforms() const;

    /**
     * Binds all specified uniforms to this material's specified shader (also binds specified textures).
     * Assumes currently bound shader is the same as the material's.
     */
    void bindUniforms() const;
};
