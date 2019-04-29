#include "Resources.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Path.hpp"
#include "Resource.h"
#include "Primitives.hpp"

#include <glad/glad.h>

#include <map>
#include <string>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>


#include <fstream>
#include <sstream>

static std::map<std::string, Texture> textureRegistry;
static std::map<std::string, Shader> shaderRegistry;
static std::map<std::string, Shader> postProcessingShaderRegistry;
static std::map<std::string, AssimpFile> assimpFileRegistry;
static std::map<std::string, Material> materialRegistry;

static std::string readShader(std::ifstream &file, std::string directory) {
    std::string source, line;
    while (std::getline(file, line))
    {
        std::string prefix = "#include \"";
        if(line.substr(0, prefix.size()) == prefix) {
            //::cout << "found include" << std::endl;

            if (line.substr(line.size() - 1) == "\"") {
                //std::cout << line.substr(prefix.size(), (line.size() - 1) - prefix.size()) << std::endl;

                std::string includePath = directory + "/" + line.substr(prefix.size(), (line.size() - 1) - prefix.size());
                std::ifstream includeFile(includePath);
                if (includeFile.is_open())
                {
                    source += readShader(includeFile, directory);
                }
                includeFile.close();
            }
        }
        else {
            source += line + "\n";
        }
    }
    return source;
}


static void load() {
    Resources::standardShader = Resources::getShader("resources/shaders/standard.vsh", "resources/shaders/standard.fsh");
    Resources::eq2cubeShader = Resources::getShader("resources/shaders/cubemap.vsh", "resources/shaders/eq2cube.fsh");
    Resources::cubemapShader = Resources::getShader("resources/shaders/cubemap.vsh", "resources/shaders/cubemap.fsh");
    Resources::irradianceShader = Resources::getShader("resources/shaders/cubemap.vsh", "resources/shaders/irradiance.fsh");
    Resources::prefilterShader = Resources::getShader("resources/shaders/cubemap.vsh", "resources/shaders/prefilter.fsh");
    Resources::passthroughShader = Resources::getPostProcessingShader("resources/shaders/passthrough.glsl");
    Resources::spriteShader = Resources::getShader("resources/shaders/sprite.vsh", "resources/shaders/sprite.fsh");
    Resources::textShader = Resources::getShader("resources/shaders/text.vsh", "resources/shaders/text.fsh");
    Resources::ShadowShader = Resources::getShader("resources/shaders/shadow.vsh", "resources/shaders/shadow.fsh");
    Resources::deferredShader = Resources::getPostProcessingShader("resources/shaders/deferred.glsl");
    Resources::deferredAmbientShader = Resources::getPostProcessingShader("resources/shaders/deferred_ambient.glsl");
    Resources::deferredPointShader = Resources::getPostProcessingShader("resources/shaders/deferred_point.glsl");
    Resources::deferredDirectionalShadowShader = Resources::getPostProcessingShader("resources/shaders/deferred_directional_shadow.glsl");
    Resources::deferredDirectionalShader = Resources::getPostProcessingShader("resources/shaders/deferred_directional.glsl");
    Resources::brdfShader = Resources::getPostProcessingShader("resources/shaders/brdf.glsl");

    Resources::debugShader = Resources::getShader("resources/shaders/debug.vsh", "resources/shaders/debug.fsh");
    Resources::particleShader = Resources::getShader("resources/shaders/particle.vsh", "resources/shaders/particle.fsh", "resources/shaders/particle.gsh");

    Resources::tonemapShader = Resources::getPostProcessingShader("resources/shaders/tonemap.glsl");
    Resources::fxaaShader = Resources::getPostProcessingShader("resources/shaders/fxaa.glsl");
    Resources::gaussianBlurShader = Resources::getPostProcessingShader("resources/shaders/gaussianBlur.glsl");
    Resources::ssaoShader = Resources::getPostProcessingShader("resources/shaders/ssao.glsl");
    Resources::ssaoBlurShader = Resources::getPostProcessingShader("resources/shaders/ssaoBlur.glsl");
    Resources::ssrShader = Resources::getPostProcessingShader("resources/shaders/ssr.glsl");
    Resources::gammaCorrectShader = Resources::getPostProcessingShader("resources/shaders/gammaCorrect.glsl");
    Resources::bloomShader = Resources::getPostProcessingShader("resources/shaders/bloom.glsl");

    Resources::framebufferMesh = Primitives::framebuffer();
    Resources::cubemapMesh = Primitives::skybox();
    Resources::spriteMesh = Primitives::sprite();


    // create brdf texture
    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    Resources::brdfShader.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Resources::framebufferMesh.render();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);

    Resources::brdf.setID(brdfLUTTexture);
}

namespace Resources {
    Mesh cubemapMesh;
    Mesh framebufferMesh;
    Mesh spriteMesh;

    Shader standardShader;
    Shader eq2cubeShader;
    Shader cubemapShader;
    Shader irradianceShader;
    Shader prefilterShader;
    Shader brdfShader;
    Shader passthroughShader;
    Shader spriteShader;
    Shader textShader;
    Shader ShadowShader;
    Shader deferredShader;
    Shader deferredAmbientShader;
    Shader deferredPointShader;
    Shader deferredDirectionalShadowShader;
    Shader deferredDirectionalShader;
    
    Shader debugShader;
    Shader particleShader;

    Shader tonemapShader;
    Shader fxaaShader;
    Shader gaussianBlurShader;
    Shader ssaoShader;
    Shader ssaoBlurShader;
    Shader ssrShader;
    Shader gammaCorrectShader;
    Shader bloomShader;

    Texture brdf;

    void loadDefaultResources() {
        load();
    }

    Texture &getTexture(const Path &path, bool pixelated) {
        if (textureRegistry.find(path) == textureRegistry.end()) {
            std::cout << "loading texture: " << path << std::endl;

            Texture texture;
            stbi_set_flip_vertically_on_load(false);
            int width, height, components;
            unsigned char* image = stbi_load(path.toString().c_str(), &width, &height, &components, STBI_rgb_alpha);

            if (image) {
                texture.load(image, width, height, pixelated, false);

                stbi_image_free(image);
            }
            else {
                std::cerr << "error loading texture: " << path << std::endl;
            }

            textureRegistry.insert(std::make_pair(path, texture));
        }
        return textureRegistry.at(path);
    }

    Shader &getShader(const Path &vertexShader, const Path &fragmentShader) {
        std::string key = vertexShader.toString()+fragmentShader.toString();

        if (shaderRegistry.find(key) == shaderRegistry.end()) {
            std::cout << "loading shader: " << vertexShader << ", " << fragmentShader << std::endl;

            Shader shader;
            Path directory = vertexShader.getParent();

            std::ifstream vertexStream(vertexShader);
            std::ifstream fragmentStream(fragmentShader);

            if (vertexStream.is_open() && fragmentStream.is_open()) {
                std::string vertexCode = readShader(vertexStream, directory);
                std::string fragmentCode = readShader(fragmentStream, directory);


                vertexStream.close();
                fragmentStream.close();

                shader.load(vertexCode, fragmentCode);
            }
            else {
                std::cerr << "error loading shader: " << vertexShader << ", " << fragmentShader << std::endl;
            }



            shaderRegistry.insert(std::make_pair(key, shader));
        }
        return shaderRegistry.at(key);
    }

    Shader &getShader(const Path &vertexShader, const Path &fragmentShader, const Path &geometryShader) {
        std::string key = vertexShader.toString()+fragmentShader.toString()+geometryShader.toString();

        if (shaderRegistry.find(key) == shaderRegistry.end()) {
            std::cout << "loading shader: " << vertexShader << ", " << fragmentShader << ", " << geometryShader << std::endl;

            Shader shader;
            Path directory = vertexShader.getParent();

            std::ifstream vertexStream(vertexShader);
            std::ifstream fragmentStream(fragmentShader);
            std::ifstream geometryStream(geometryShader);

            if (vertexStream.is_open() && fragmentStream.is_open() && geometryStream.is_open()) {
                std::string vertexCode = readShader(vertexStream, directory);
                std::string fragmentCode = readShader(fragmentStream, directory);
                std::string geometryCode = readShader(geometryStream, directory);


                vertexStream.close();
                fragmentStream.close();
                geometryStream.close();

                shader.load(vertexCode, fragmentCode, geometryCode);
            }
            else {
                std::cerr << "error loading shader: " << vertexShader << ", " << fragmentShader << ", " << geometryShader << std::endl;
            }

            shaderRegistry.insert(std::make_pair(key, shader));
        }
        return shaderRegistry.at(key);
    }

    Shader &getPostProcessingShader(const Path &path) {
        std::string key = path.toString();

        if (postProcessingShaderRegistry.find(key) == postProcessingShaderRegistry.end()) {
            std::cout << "loading post processing shader: " << path << std::endl;

            Shader shader;
            Path directory = path.getParent();

            std::ifstream stream(path);

            if (stream.is_open()) {
                std::string code = readShader(stream, directory);


                stream.close();

                shader.loadPostProcessing(code);
            }
            else {
                std::cerr << "error loading post processing shader: " << path << std::endl;
            }



            postProcessingShaderRegistry.insert(std::make_pair(key, shader));
        }
        return postProcessingShaderRegistry.at(key);
    }

    AssimpFile &getAssimpFile(const Path &path) {
        // if (assimpFileRegistry.find(path) == assimpFileRegistry.end()) {
        //     std::cout << "loading Assimp file: " << path << std::endl;

        //     const aiScene* scene = importer.ReadFile(path.toString(), aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices);

        //     if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        //     {
        //         std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        //     }

        //     assimpFileRegistry.insert(std::make_pair(path, AssimpFile()));
        //     assimpFileRegistry.at(path).load(scene, path.getParent());
        // }

        return assimpFileRegistry.at(path);
    }

    Material &getMaterial(const Path &path) {
        if (materialRegistry.find(path) == materialRegistry.end()) {
            std::cout << "loading material: " << path << std::endl;
            Material mat;

            Path wd = path.getParent();


            std::ifstream o(path);
            if (o.is_open()) {
                json j;

                o >> j;

                mat.fromJson(j, wd);
            }
            else {
                std::cerr << "error loading material: " << path <<  std::endl;
            }


            materialRegistry.insert(std::make_pair(path, mat));
        }

        return materialRegistry.at(path);
    }
}