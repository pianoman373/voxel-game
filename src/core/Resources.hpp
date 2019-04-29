#pragma once

#include "Path.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "AssimpFile.hpp"
#include "Material.hpp"

namespace Resources {
    extern Mesh cubemapMesh;
    extern Mesh framebufferMesh;
    extern Mesh spriteMesh;

    extern Shader standardShader;
    extern Shader eq2cubeShader;
    extern Shader cubemapShader;
    extern Shader irradianceShader;
    extern Shader prefilterShader;
    extern Shader brdfShader;
    extern Shader passthroughShader;
    extern Shader spriteShader;
    extern Shader textShader;
    extern Shader ShadowShader;
    extern Shader deferredShader;
    extern Shader deferredAmbientShader;
    extern Shader deferredPointShader;
    extern Shader deferredDirectionalShadowShader;
    extern Shader deferredDirectionalShader;

    extern Shader debugShader;
    extern Shader particleShader;

    extern Shader tonemapShader;
    extern Shader fxaaShader;
    extern Shader gaussianBlurShader;
    extern Shader ssaoShader;
    extern Shader ssaoBlurShader;
    extern Shader ssrShader;
    extern Shader gammaCorrectShader;
    extern Shader bloomShader;



    extern Texture brdf;


    void loadDefaultResources();

    Texture &getTexture(const Path &path, bool pixelated=false);

    Shader &getShader(const Path &vertexShader, const Path &fragmentShader);

    Shader &getShader(const Path &vertexShader, const Path &fragmentShader, const Path &geometryShader);

    Shader &getPostProcessingShader(const Path &path);

    AssimpFile &getAssimpFile(const Path &path);

    Material &getMaterial(const Path &path);
}