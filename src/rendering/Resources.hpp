#pragma once

#include "util/Path.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Texture.hpp"
#include "rendering/Material.hpp"
#include "rendering/Mesh.hpp"

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

    extern Shader tonemapShader;
    extern Shader fxaaShader;
    extern Shader gaussianBlurShader;
    extern Shader ssaoShader;
    extern Shader ssaoBlurShader;
    extern Shader gammaCorrectShader;
    extern Shader bloomShader;
    extern Shader fogShader;



    extern Texture brdf;


    void loadDefaultResources();
}