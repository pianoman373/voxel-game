#include "DirectionalLight.hpp"
#include "Frustum.hpp"
#include "Resources.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"

#include <glad/glad.h>

Camera DirectionalLight::getShadowCamera(float radius, const Camera &cam, float depth) {
    Camera ret;
    ret.position = cam.getPosition();
    ret.direction = m_direction;
    ret.up = vec3(0.0f, 1.0f, 0.0f);
    ret.dimensions = vec2(radius*1.01*2.0f, radius*1.01*2.0f);

    ret.nearPlane = -depth;
    ret.farPlane = depth;

    ret.orthographic = true;
    
    return ret;
}

Frustum DirectionalLight::getShadowFrustum(float radius, const Camera &cam, float depth) {
	Frustum shadowFrustum;
	shadowFrustum.setupInternalsOrthographic(-radius*1.01, radius*1.01, -radius*1.01, radius*1.01, -depth, depth);
	Camera shadowCam;
	shadowCam.setPosition(cam.getPosition());
	shadowCam.setDirection(m_direction);
	shadowFrustum.updateCamPosition(shadowCam);

	return shadowFrustum;
}

void DirectionalLight::setupFramebuffers() {
    shadowBuffers = {Framebuffer(), Framebuffer(), Framebuffer(), Framebuffer()};

    for (int i = 0; i < m_shadowDistances.size(); i++) {
        shadowBuffers.push_back(Framebuffer());

        shadowBuffers[i].setup(m_shadowResolution, m_shadowResolution);
        shadowBuffers[i].attachShadow(m_shadowResolution, m_shadowResolution);
    }
}

DirectionalLight::DirectionalLight(vec3 direction, vec3 color) {
    m_direction = direction;
    m_color = color;
    m_shadowResolution = 0;
    m_hasShadows = false;
    m_shadowDepth = 0;
}

DirectionalLight::DirectionalLight(vec3 direction, vec3 color, int shadowResolution) {
    m_direction = direction;
    m_color = color;
    m_shadowResolution = shadowResolution;
    m_hasShadows = true;

    m_shadowDepth = 400.0f;
    m_shadowDistances = { 20.0f, 60.0f, 150.0f, 400.0f };
}

DirectionalLight::DirectionalLight(vec3 direction, vec3 color, int shadowResolution, int numCascades, float farDistance) {
    m_direction = direction;
    m_color = color;
    m_shadowResolution = shadowResolution;
    m_hasShadows = true;

    float cascadeSplitLambda = 0.6f;

    float nearClip = 0.1f;
    float farClip = farDistance;
    float clipRange = farClip - nearClip;

    float minZ = nearClip;
    float maxZ = nearClip + clipRange;

    float range = maxZ - minZ;
    float ratio = maxZ / minZ;

    // Calculate split depths based on view camera furstum
    // Based on method presentd in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
    for (uint32_t i = 0; i < numCascades; i++) {
        float p = (i + 1) / static_cast<float>(numCascades);
        float log = minZ * std::pow(ratio, p);
        float uniform = minZ + range * p;
        float d = cascadeSplitLambda * (log - uniform) + uniform;
        m_shadowDistances.push_back((d - nearClip));
    }

    m_shadowDepth = m_shadowDistances.back();
}

DirectionalLight::DirectionalLight(vec3 direction, vec3 color, int shadowResolution, std::vector<float> distances) {
    m_direction = direction;
    m_color = color;
    m_shadowResolution = shadowResolution;
    m_hasShadows = true;

    m_shadowDistances = distances;
    m_shadowDepth = m_shadowDistances[m_shadowDistances.size()];
}

void DirectionalLight::preRender(const Camera &cam) {
    if (m_hasShadows) {
        if (!isFramebufferSetup) {
            setupFramebuffers();
            isFramebufferSetup = true;
        }

        for (int i = 0; i < m_shadowDistances.size(); i++) {
            Camera shadowCamera = getShadowCamera(m_shadowDistances[i], cam, m_shadowDepth);
            Frustum shadowFrustum = getShadowFrustum(m_shadowDistances[i], cam, m_shadowDepth);

            Renderer::renderToDepth(shadowBuffers[i], shadowCamera, shadowFrustum, false);
        }
    }
}

void DirectionalLight::render(const Camera &cam) {
    if (m_hasShadows) {
        Resources::deferredDirectionalShadowShader.bind();

        mat4 inverseView = inverse(cam.getView());

        Renderer::getGBuffer().getAttachment(0).bind(0);
        Renderer::getGBuffer().getAttachment(1).bind(1);
        Renderer::getGBuffer().getAttachment(2).bind(2);
        Renderer::getGBuffer().getAttachment(3).bind(3);

        Resources::deferredDirectionalShadowShader.uniformInt("gPosition", 0);
        Resources::deferredDirectionalShadowShader.uniformInt("gNormal", 1);
        Resources::deferredDirectionalShadowShader.uniformInt("gAlbedo", 2);
        Resources::deferredDirectionalShadowShader.uniformInt("gRoughnessMetallic", 3);

        Resources::deferredDirectionalShadowShader.uniformVec3("sun.direction", vec3(vec4(m_direction, 0.0f) * cam.getView()));
        Resources::deferredDirectionalShadowShader.uniformVec3("sun.color", m_color);
        Resources::deferredDirectionalShadowShader.uniformMat4("view", cam.getView());

        mat4 biasMatrix = mat4(
            0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f
        );

        for (int i = 0; i < m_shadowDistances.size(); i++) {
            shadowBuffers[i].getAttachment(0).bind(8+i);
            Resources::deferredDirectionalShadowShader.uniformInt("shadowTextures["+std::to_string(i)+"]", 8+i);

            Camera shadowCamera = getShadowCamera(m_shadowDistances[i], cam, m_shadowDepth);

            Resources::deferredDirectionalShadowShader.uniformMat4("lightSpaceMatrix["+std::to_string(i)+"]", biasMatrix * shadowCamera.getProjection() * shadowCamera.getView() * inverseView);
            Resources::deferredDirectionalShadowShader.uniformFloat("shadowDistances["+std::to_string(i)+"]", m_shadowDistances[i]);
        }

        Resources::deferredDirectionalShadowShader.uniformInt("numCascades", m_shadowDistances.size());

        Resources::framebufferMesh.render();
    }
    else {
        Resources::deferredDirectionalShader.bind();

        mat4 inverseView = inverse(cam.getView());

        Renderer::getGBuffer().getAttachment(0).bind(0);
        Renderer::getGBuffer().getAttachment(1).bind(1);
        Renderer::getGBuffer().getAttachment(2).bind(2);
        Renderer::getGBuffer().getAttachment(3).bind(3);

        Resources::deferredDirectionalShader.uniformInt("gPosition", 0);
        Resources::deferredDirectionalShader.uniformInt("gNormal", 1);
        Resources::deferredDirectionalShader.uniformInt("gAlbedo", 2);
        Resources::deferredDirectionalShader.uniformInt("gRoughnessMetallic", 3);

        Resources::deferredDirectionalShader.uniformVec3("sun.direction", vec3(vec4(m_direction, 0.0f) * cam.getView()));
        Resources::deferredDirectionalShader.uniformVec3("sun.color", m_color);
        Resources::deferredDirectionalShader.uniformMat4("view", cam.getView());

        Resources::framebufferMesh.render();
    }
}