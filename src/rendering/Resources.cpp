#include "rendering/Resources.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Texture.hpp"
#include "util/Path.hpp"
#include "rendering/Primitives.hpp"

#include <glad/glad.h>

#include <map>
#include <string>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>


#include <fstream>
#include <sstream>

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
    Shader deferredDirectionalShadowShader;
    Shader deferredDirectionalShader;
    
    Shader debugShader;
    Shader particleShader;

    Shader tonemapShader;
    Shader fxaaShader;
    Shader gaussianBlurShader;
    Shader ssaoShader;
    Shader ssaoBlurShader;
    Shader gammaCorrectShader;
    Shader bloomShader;
    Shader fogShader;

    Texture brdf;

    void loadDefaultResources() {
        standardShader.loadFile("mods/base/resources/shaders/standard.glsl");
        eq2cubeShader.loadFile("mods/base/resources/shaders/eq2cube.glsl");
        cubemapShader.loadFile("mods/base/resources/shaders/cubemap.glsl");
        irradianceShader.loadFile("mods/base/resources/shaders/irradiance.glsl");
        prefilterShader.loadFile("mods/base/resources/shaders/prefilter.glsl");

        passthroughShader.loadFile("mods/base/resources/shaders/passthrough.glsl");
        

        spriteShader.loadFile("mods/base/resources/shaders/sprite.glsl");
        textShader.loadFile("mods/base/resources/shaders/text.glsl");
        ShadowShader.loadFile("mods/base/resources/shaders/shadow.glsl");
        deferredShader.loadFile("mods/base/resources/shaders/deferred.glsl");
        deferredAmbientShader.loadFile("mods/base/resources/shaders/deferred_ambient.glsl");
        deferredDirectionalShadowShader.loadFile("mods/base/resources/shaders/deferred_directional_shadow.glsl");
        deferredDirectionalShader.loadFile("mods/base/resources/shaders/deferred_directional.glsl");
        brdfShader.loadFile("mods/base/resources/shaders/brdf.glsl");

        debugShader.loadFile("mods/base/resources/shaders/debug.glsl");

        tonemapShader.loadFile("mods/base/resources/shaders/tonemap.glsl");

        fxaaShader.loadFile("mods/base/resources/shaders/fxaa.glsl");
        gaussianBlurShader.loadFile("mods/base/resources/shaders/gaussianBlur.glsl");
        ssaoShader.loadFile("mods/base/resources/shaders/ssao.glsl");
        ssaoBlurShader.loadFile("mods/base/resources/shaders/ssaoBlur.glsl");
        gammaCorrectShader.loadFile("mods/base/resources/shaders/gammaCorrect.glsl");
        bloomShader.loadFile("mods/base/resources/shaders/bloom.glsl");
        fogShader.loadFile("mods/base/resources/shaders/fog.glsl");

        framebufferMesh = Primitives::framebuffer();
        cubemapMesh = Primitives::skybox();
        spriteMesh = Primitives::sprite();


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
        brdfShader.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        framebufferMesh.render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glDeleteFramebuffers(1, &captureFBO);
        glDeleteRenderbuffers(1, &captureRBO);

        brdf.setID(brdfLUTTexture);
    }
}