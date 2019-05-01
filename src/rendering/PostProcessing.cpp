#include "rendering/PostProcessing.hpp"
#include "rendering/Renderer.hpp"
#include "client/Window.hpp"
#include "rendering/Resources.hpp"

#include <glad/glad.h>

#include <random>

void PostProcessor::postProcess(const Camera &, const Framebuffer &source, const Framebuffer &destination) {
    destination.bind();
    Resources::getPostProcessingShader("resources/invert.glsl").bind();

    source.getAttachment(0).bind();
    Resources::framebufferMesh.render();
}

void PostProcessor::resize() {

}

void FxaaPostProcessor::postProcess(const Camera &, const Framebuffer &source, const Framebuffer &destination) {
    destination.bind();
    Resources::fxaaShader.bind();
    Resources::fxaaShader.uniformInt("source", 0);

    source.getAttachment(0).bind();
    Resources::framebufferMesh.render();
}

void TonemapPostProcessor::postProcess(const Camera &, const Framebuffer &source, const Framebuffer &destination) {
    destination.bind();
    Resources::tonemapShader.bind();
    Resources::tonemapShader.uniformInt("source", 0);

    source.getAttachment(0).bind();
    Resources::framebufferMesh.render();
}

void FogPostProcessor::postProcess(const Camera &, const Framebuffer &source, const Framebuffer &destination) {
    destination.bind();
    Resources::fogShader.bind();
    Resources::fogShader.uniformInt("source", 0);
    Renderer::getGBuffer().getAttachment(0).bind(1);
    Resources::fogShader.uniformInt("gPosition", 1);

    Resources::fogShader.uniformFloat("fogInner", fogInner);
    Resources::fogShader.uniformFloat("fogOuter", fogOuter);

    source.getAttachment(0).bind();
    Resources::framebufferMesh.render();
}


SsaoPostProcessor::SsaoPostProcessor() {
    //setup SSAO
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between 0.0 - 1.0
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 256; ++i)
    {
        vec3 sample(
                randomFloats(generator) * 2.0 - 1.0,
                randomFloats(generator) * 2.0 - 1.0,
                randomFloats(generator)
        );
        sample  = normalize(sample);
        sample = sample * randomFloats(generator);
        float scale = (float)i / 256.0;

        scale   = lerp(0.1f, 1.0f, scale * scale);
        sample = sample * scale;
        ssaoKernel.push_back(sample);
    }

    std::vector<vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    unsigned int noiseTexture;
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    noiseTex.setID(noiseTexture);

    resize();
}

void SsaoPostProcessor::postProcess(const Camera &cam, const Framebuffer &source, const Framebuffer &destination) {
    Framebuffer &gBuffer = Renderer::getGBuffer();
    vec2i resolution = Renderer::getResolution();

    // render the g-buffers for SSAO
    // ---------------------------------------------
    glViewport(0, 0, ssaoBuffer.getWidth(), ssaoBuffer.getHeight());
    ssaoBuffer.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Resources::ssaoShader.bind();

    Resources::ssaoShader.uniformInt("gPosition", 0);
    gBuffer.getAttachment(0).bind(0);

    Resources::ssaoShader.uniformInt("gNormal", 1);
    gBuffer.getAttachment(1).bind(1);

    Resources::ssaoShader.uniformInt("texNoise", 2);
    noiseTex.bind(2);

    Resources::ssaoShader.uniformMat4("projection", cam.getProjection());
    Resources::ssaoShader.uniformFloat("radius", ssaoRadius);
    Resources::ssaoShader.uniformFloat("strength", strength);
    Resources::ssaoShader.uniformInt("kernelSize", ssaoKernelSize);

    Resources::ssaoShader.uniformVec3("noiseScale",  vec3(ssaoBuffer.getWidth()/4.0f, ssaoBuffer.getHeight()/4.0f, 0.0f));

    for (int i = 0; i < ssaoKernelSize; i++) {
        Resources::ssaoShader.uniformVec3(std::string("samples[") + std::to_string(i) + std::string("]"), ssaoKernel[i]);
    }
    Resources::framebufferMesh.render();

    glViewport(0, 0, resolution.x, resolution.y);
    destination.bind();
    Resources::ssaoBlurShader.bind();
    ssaoBuffer.getAttachment(0).bind(0);
    Resources::ssaoBlurShader.uniformInt("ssao", 0);
    source.getAttachment(0).bind(1);
    Resources::ssaoBlurShader.uniformInt("source", 1);
    Resources::framebufferMesh.render();
}

void SsaoPostProcessor::resize() {
    vec2i resolution = Renderer::getResolution();

    ssaoBuffer.destroy();
    if (highQuality) {
        ssaoBuffer.setup(resolution.x, resolution.y);
    }
    else {
        ssaoBuffer.setup(resolution.x/2, resolution.y/2);
    }
    
    ssaoBuffer.attachTexture(GL_RED, GL_RED, GL_UNSIGNED_BYTE);
}


static float gaussianDistribution (float x, float sigma)
{
    float d = x;
    float n = 1.0f / (sqrt(2.0f * PI) * sigma);

    return exp(-d*d/(2.0f * sigma * sigma)) * n;
};

static void uniformGaussians(const Shader &s, std::string name, int radius) {
    for (int i = 0; i < radius; i++) {
        s.uniformFloat(name + "[" + std::to_string(i) + "]", gaussianDistribution(i, 1.0f));
    }

    s.uniformInt(name + "_length", radius);
}

BloomPostProcessor::BloomPostProcessor() {
    resize();
}

void BloomPostProcessor::postProcess(const Camera &, const Framebuffer &source, const Framebuffer &destination) {
    vec2i resolution = Renderer::getResolution();

    int blurRadius = 8;

    glViewport(0, 0, bloomBuffer0.getWidth(), bloomBuffer0.getHeight());
    bloomBuffer1.bind();
    Resources::passthroughShader.bind();
    source.getAttachment(0).bind();
    Resources::framebufferMesh.render();

    bloomBuffer0.bind();
    Resources::gaussianBlurShader.bind();
    Resources::gaussianBlurShader.uniformBool("horizontal", true);
    uniformGaussians(Resources::gaussianBlurShader, "weights", blurRadius);
    bloomBuffer1.getAttachment(0).bind();
    Resources::framebufferMesh.render();

    bloomBuffer1.bind();
    Resources::gaussianBlurShader.bind();
    Resources::gaussianBlurShader.uniformBool("horizontal", false);
    uniformGaussians(Resources::gaussianBlurShader, "weights", blurRadius);
    bloomBuffer0.getAttachment(0).bind();
    Resources::framebufferMesh.render();

    // -------------------------------------------------------------------------

    glViewport(0, 0, bloomBuffer3.getWidth(), bloomBuffer3.getHeight());
    bloomBuffer3.bind();
    Resources::passthroughShader.bind();
    bloomBuffer1.getAttachment(0).bind();
    Resources::framebufferMesh.render();

    bloomBuffer2.bind();
    Resources::gaussianBlurShader.bind();
    Resources::gaussianBlurShader.uniformBool("horizontal", true);
    uniformGaussians(Resources::gaussianBlurShader, "weights", blurRadius);
    bloomBuffer3.getAttachment(0).bind();
    Resources::framebufferMesh.render();

    bloomBuffer3.bind();
    Resources::gaussianBlurShader.bind();
    Resources::gaussianBlurShader.uniformBool("horizontal", false);
    uniformGaussians(Resources::gaussianBlurShader, "weights", blurRadius);
    bloomBuffer2.getAttachment(0).bind();
    Resources::framebufferMesh.render();

    // -------------------------------------------------------------------------

    glViewport(0, 0, bloomBuffer5.getWidth(), bloomBuffer5.getHeight());
    bloomBuffer5.bind();
    Resources::passthroughShader.bind();
    bloomBuffer3.getAttachment(0).bind();
    Resources::framebufferMesh.render();

    bloomBuffer4.bind();
    Resources::gaussianBlurShader.bind();
    Resources::gaussianBlurShader.uniformBool("horizontal", true);
    uniformGaussians(Resources::gaussianBlurShader, "weights", blurRadius);
    bloomBuffer5.getAttachment(0).bind();
    Resources::framebufferMesh.render();

    bloomBuffer5.bind();
    Resources::gaussianBlurShader.bind();
    Resources::gaussianBlurShader.uniformBool("horizontal", false);
    uniformGaussians(Resources::gaussianBlurShader, "weights", blurRadius);
    bloomBuffer4.getAttachment(0).bind();
    Resources::framebufferMesh.render();


    
    glViewport(0, 0, resolution.x, resolution.y);
    destination.bind();
    Resources::bloomShader.bind();

    source.getAttachment(0).bind(0);
    Resources::bloomShader.uniformInt("source", 0);

    bloomBuffer1.getAttachment(0).bind(1);
    Resources::bloomShader.uniformInt("bloom0", 1);

    bloomBuffer3.getAttachment(0).bind(2);
    Resources::bloomShader.uniformInt("bloom1", 2);

    bloomBuffer5.getAttachment(0).bind(3);
    Resources::bloomShader.uniformInt("bloom2", 3);

    Resources::bloomShader.uniformFloat("bloomStrength", bloomStrength);

    Resources::framebufferMesh.render();
}

void BloomPostProcessor::resize() {
    vec2i resolution = Renderer::getResolution();

    //bloom framebuffers
    bloomBuffer0.destroy();
    bloomBuffer0.setup(resolution.x/2, resolution.y/2);
    bloomBuffer0.attachTexture(GL_RGB16F, GL_RGB, GL_FLOAT);
    bloomBuffer1.destroy();
    bloomBuffer1.setup(resolution.x/2, resolution.y/2);
    bloomBuffer1.attachTexture(GL_RGB16F, GL_RGB, GL_FLOAT);

    bloomBuffer2.destroy();
    bloomBuffer2.setup(resolution.x/8, resolution.y/8);
    bloomBuffer2.attachTexture(GL_RGB16F, GL_RGB, GL_FLOAT);
    bloomBuffer3.destroy();
    bloomBuffer3.setup(resolution.x/8, resolution.y/8);
    bloomBuffer3.attachTexture(GL_RGB16F, GL_RGB, GL_FLOAT);

    bloomBuffer4.destroy();
    bloomBuffer4.setup(resolution.x/16, resolution.y/16);
    bloomBuffer4.attachTexture(GL_RGB16F, GL_RGB, GL_FLOAT);
    bloomBuffer5.destroy();
    bloomBuffer5.setup(resolution.x/16, resolution.y/16);
    bloomBuffer5.attachTexture(GL_RGB16F, GL_RGB, GL_FLOAT);
}