#pragma once

#include "Math.hpp"
#include "Framebuffer.hpp"
#include "Camera.hpp"
#include "Frustum.hpp"

class DirectionalLight {
private:
    bool isFramebufferSetup = false;

    Camera getShadowCamera(float radius, const Camera &cam, float depth);

    Frustum getShadowFrustum(float radius, const Camera &cam, float depth);

    void setupFramebuffers();

    bool m_hasShadows;
    int m_shadowResolution;

    float m_shadowDepth;
    std::vector<float> m_shadowDistances;

    std::vector<Framebuffer> shadowBuffers;

public:
    vec3 m_direction;
    vec3 m_color;

    DirectionalLight(vec3 direction, vec3 color);

    DirectionalLight(vec3 direction, vec3 color, int shadowResolution);

    DirectionalLight(vec3 direction, vec3 color, int shadowResolution, int numCascades, float farDistance);

    DirectionalLight(vec3 direction, vec3 color, int shadowResolution, std::vector<float> distances);

    

    void preRender(const Camera &cam);

    void render(const Camera &cam);
};