#pragma once

#include "rendering/Framebuffer.hpp"
#include "rendering/Camera.hpp"
#include "rendering/Shader.hpp"

class PostProcessor {
public:
    virtual void postProcess(const Camera &cam, const Framebuffer &source, const Framebuffer &destination);

    virtual void resize();
};

class FxaaPostProcessor: public PostProcessor {
public:
    void postProcess(const Camera &cam, const Framebuffer &source, const Framebuffer &destination);
};

class TonemapPostProcessor: public PostProcessor {
public:
    void postProcess(const Camera &cam, const Framebuffer &source, const Framebuffer &destination);
};

class FogPostProcessor: public PostProcessor {
public:
    float fogInner = 90.0f;
    float fogOuter = 100.0f;

    void postProcess(const Camera &cam, const Framebuffer &source, const Framebuffer &destination);
};

class SsaoPostProcessor: public PostProcessor {
private:
    Framebuffer ssaoBuffer;

    std::vector<vec3> ssaoKernel;
    Texture noiseTex;

public:
    float ssaoRadius = 4.0f;
    float strength = 1.0f;
    int ssaoKernelSize = 8;
    bool highQuality = true;

    SsaoPostProcessor();

    virtual void postProcess(const Camera &cam, const Framebuffer &source, const Framebuffer &destination);

    virtual void resize();
};


class BloomPostProcessor: public PostProcessor {
private:
    Framebuffer bloomBuffer0;
    Framebuffer bloomBuffer1;
    Framebuffer bloomBuffer2;
    Framebuffer bloomBuffer3;
    Framebuffer bloomBuffer4;
    Framebuffer bloomBuffer5;

public:
    float bloomStrength = 3.0f;

    BloomPostProcessor();

    virtual void postProcess(const Camera &cam, const Framebuffer &source, const Framebuffer &destination);

    virtual void resize();
};