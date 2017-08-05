#include "Renderer.hpp"
#include "Framebuffer.hpp"
#include <glad/glad.h>
#include "Window.hpp"
#include "Camera.hpp"
#include "AABB.hpp"
#include "Frustum.hpp"
#include "Mesh.hpp"
#include "MeshFactory.hpp"

#include <imgui.h>
#include <stack>
#include <string>

const float cascadeDistances[4] = {10.0f, 40.0f, 100.0f, 500.0f};
const vec3 sunDirection = normalize(vec3(0.4f, 0.7f, 1.0f));
const vec3 sunColor = vec3(1.4f, 1.3f, 1.0f);

std::stack<RenderCall> renderQueue;

Framebuffer shadowBuffer0;
Framebuffer shadowBuffer1;
Framebuffer shadowBuffer2;
Framebuffer shadowBuffer3;

Shader ShadowShader;
Shader debugShader;

Mesh skyboxMesh;
Mesh crosshairMesh;
MeshFactory ms;

Mesh mesh;

static const std::string debug_vsh = R"(
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 view;
uniform mat4 projection;

out vec3 vertexColor;

void main() {
	gl_Position = projection * view * vec4(position, 1.0);
	vertexColor = color;
}
)";

static const std::string debug_fsh = R"(
#version 330 core

uniform mat4 view;
uniform mat4 projection;

in vec3 vertexColor;

out vec4 FragColor;

void main() {
    FragColor = vec4(vertexColor, 1.0);
}
)";

static bool shadows;
static int shadow_resolution;

static mat4 shadowMatrix(float radius, Camera &cam) {
    return orthographic(-radius, radius, -radius, radius, -radius * 8.0f, radius * 8.0f) * LookAt(cam.getPosition() + sunDirection, cam.getPosition(), vec3( 0.0f, 1.0f,  0.0f));
}

static Frustum shadowFrustum(float radius, Camera &cam) {
    Frustum shadowFrustum;
    shadowFrustum.setupInternalsOrthographic(-radius, radius, -radius, radius, -radius * 8, radius * 8);
    Camera shadowCam;
    shadowCam.setPosition(cam.getPosition());
    shadowCam.setDirection(sunDirection);
    shadowFrustum.updateCamPosition(shadowCam);

    return shadowFrustum;
}

static void renderShadow(Framebuffer &fbuffer, mat4 lightSpaceMatrix, Frustum f) {
    glViewport(0, 0, fbuffer.getWidth(), fbuffer.getHeight());
    fbuffer.bind();
    glClear(GL_DEPTH_BUFFER_BIT);

    std::stack<RenderCall> renderQueueCopy = renderQueue;

    for (unsigned int i = 0; i < renderQueueCopy.size(); i++) {
        RenderCall c = renderQueueCopy.top();
        renderQueueCopy.pop();

        if (f.isBoxInside(c.aabb)) {

            ShadowShader.bind();
            ShadowShader.uniformMat4("lightSpaceMatrix", lightSpaceMatrix);

            mat4 model;
            model = translate(model, c.transform.position);
            ShadowShader.uniformMat4("model", model);

            c.mesh->render();
        }
    }
}

void Renderer::init(bool doShadows, int shadowResolution) {
    shadows = doShadows;
    shadow_resolution = shadowResolution;

    if (shadows) {
        shadowBuffer0.setupShadow(shadow_resolution, shadow_resolution);
        shadowBuffer1.setupShadow(shadow_resolution, shadow_resolution);
        shadowBuffer2.setupShadow(shadow_resolution, shadow_resolution);
        shadowBuffer3.setupShadow(shadow_resolution, shadow_resolution);
        ShadowShader.loadFile("resources/shadow.vsh", "resources/shadow.fsh");
    }

    debugShader.load(debug_vsh, debug_fsh);
}

void Renderer::render(Mesh *mesh, Material *material, Transform transform, AABB aabb) {
    RenderCall call;
    call.mesh = mesh;
    call.material = material;
    call.transform = transform;
    call.aabb = aabb;

    renderQueue.push(call);
}

void Renderer::renderDebugLine(vec3 v1, vec3 v2, vec3 color) {
    ms.vertex(v1.x, v1.y, v1.z, color.x, color.y, color.z, 0.0f, 0.0f);
    ms.vertex(v2.x, v2.y, v2.z, color.x, color.y, color.z, 0.0f, 0.0f);
}

void Renderer::renderDebugAABB(vec3 v1, vec3 v2, vec3 color) {
    //top square
    renderDebugLine(vec3(v1.x, v1.y, v1.z), vec3(v2.x, v1.y, v1.z), color);
    renderDebugLine(vec3(v1.x, v1.y, v1.z), vec3(v1.x, v1.y, v2.z), color);
    renderDebugLine(vec3(v2.x, v1.y, v1.z), vec3(v2.x, v1.y, v2.z), color);
    renderDebugLine(vec3(v1.x, v1.y, v2.z), vec3(v2.x, v1.y, v2.z), color);

    //bottom square
    renderDebugLine(vec3(v2.x, v2.y, v2.z), vec3(v1.x, v2.y, v2.z), color);
    renderDebugLine(vec3(v2.x, v2.y, v2.z), vec3(v2.x, v2.y, v1.z), color);
    renderDebugLine(vec3(v1.x, v2.y, v1.z), vec3(v1.x, v2.y, v2.z), color);
    renderDebugLine(vec3(v1.x, v2.y, v1.z), vec3(v2.x, v2.y, v1.z), color);

    //top/bottom square connections
    renderDebugLine(vec3(v1.x, v1.y, v1.z), vec3(v1.x, v2.y, v1.z), color);
    renderDebugLine(vec3(v2.x, v2.y, v2.z), vec3(v2.x, v1.y, v2.z), color);
    renderDebugLine(vec3(v2.x, v1.y, v1.z), vec3(v2.x, v2.y, v1.z), color);
    renderDebugLine(vec3(v1.x, v1.y, v2.z), vec3(v1.x, v2.y, v2.z), color);
}

void Renderer::renderDebugAABB(AABB aabb, vec3 color) {
    renderDebugAABB(aabb.min, aabb.max, color);
}

void Renderer::flush(Camera cam) {

    mat4 lightSpaceMatrix0 = shadowMatrix(cascadeDistances[0], cam);
    mat4 lightSpaceMatrix1 = shadowMatrix(cascadeDistances[1], cam);
    mat4 lightSpaceMatrix2 = shadowMatrix(cascadeDistances[2], cam);
    mat4 lightSpaceMatrix3 = shadowMatrix(cascadeDistances[3], cam);

    Frustum shadowFrustum0 = shadowFrustum(cascadeDistances[0], cam);
    Frustum shadowFrustum1 = shadowFrustum(cascadeDistances[1], cam);
    Frustum shadowFrustum2 = shadowFrustum(cascadeDistances[2], cam);
    Frustum shadowFrustum3 = shadowFrustum(cascadeDistances[3], cam);

    //shadowFrustum0.renderDebug();
    //shadowFrustum1.renderDebug();
    //shadowFrustum2.renderDebug();
    //shadowFrustum3.renderDebug();

    //shadow buffer
    if (shadows) {
        renderShadow(shadowBuffer0, lightSpaceMatrix0, shadowFrustum0);
        renderShadow(shadowBuffer1, lightSpaceMatrix1, shadowFrustum1);
        renderShadow(shadowBuffer2, lightSpaceMatrix2, shadowFrustum2);
        renderShadow(shadowBuffer3, lightSpaceMatrix3, shadowFrustum3);
    }

    //normal rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    vec2i size = Window::getWindowSize();
    glViewport(0, 0, size.x, size.y);

//    glDepthMask(false);
//    skyboxShader.bind();
//
//    mat4 skyboxModel;
//    skyboxModel = translate(skyboxModel, cam.getPosition());
//    skyboxShader.uniformMat4("model", skyboxModel);
//    skyboxShader.uniformMat4("view", cam.getView());
//    skyboxShader.uniformMat4("projection", cam.getProjection());
//    skyboxShader.uniformVec3("sunDirection", sunDirection);
//    skyboxShader.uniformVec3("sunColor", sunColor);
//
//    skyboxMesh.render();
//    glDepthMask(true);

    Material *lastMaterial = nullptr;

    for (unsigned int i = 0; i < renderQueue.size(); i++) {
            RenderCall call = renderQueue.top();
            renderQueue.pop();

            Shader s = call.material->getShader();

            if (call.material != lastMaterial) {
                s.bind();
                call.material->bindUniforms();

                if (shadows) {
                    shadowBuffer0.bindTexture(0);
                    shadowBuffer1.bindTexture(1);
                    shadowBuffer2.bindTexture(2);
                    shadowBuffer3.bindTexture(3);

                    s.uniformInt("tex0", 0);
                    s.uniformInt("tex1", 1);
                    s.uniformInt("tex2", 2);
                    s.uniformInt("tex3", 3);

                    s.uniformMat4("lightSpaceMatrix[0]", lightSpaceMatrix0);
                    s.uniformMat4("lightSpaceMatrix[1]", lightSpaceMatrix1);
                    s.uniformMat4("lightSpaceMatrix[2]", lightSpaceMatrix2);
                    s.uniformMat4("lightSpaceMatrix[3]", lightSpaceMatrix3);

                    s.uniformFloat("cascadeDistances[0]", cascadeDistances[0]);
                    s.uniformFloat("cascadeDistances[1]", cascadeDistances[1]);
                    s.uniformFloat("cascadeDistances[2]", cascadeDistances[2]);
                    s.uniformFloat("cascadeDistances[3]", cascadeDistances[3]);
                }

                s.uniformMat4("view", cam.getView());
                s.uniformMat4("projection", cam.getProjection());
                s.uniformVec3("cameraPos", cam.getPosition());
                s.uniformVec3("sunDirection", sunDirection);
                s.uniformVec3("sunColor", sunColor);
            }

            mat4 model;
            model = translate(model, call.transform.position);
            s.uniformMat4("model", model);

            call.mesh->render();

            lastMaterial = call.material;
    }

    debugShader.bind();
    debugShader.uniformMat4("view", mat4());
    debugShader.uniformMat4("projection", orthographic(-(size.x/2), (size.x/2), (size.y/2), -(size.y/2), -1.0f, 1.0f));
    crosshairMesh.render();

    ms.toMesh(mesh);

    debugShader.bind();
    debugShader.uniformMat4("view", cam.getView());
    debugShader.uniformMat4("projection", cam.getProjection());
    mesh.render(GL_LINES);

    ms.clear();


    ImGui::Image(ImTextureID(shadowBuffer0.getTextureID()), ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0), ImColor(255,255,255,255), ImColor(255,255,255,128));
    ImGui::Image(ImTextureID(shadowBuffer1.getTextureID()), ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0), ImColor(255,255,255,255), ImColor(255,255,255,128));
    ImGui::Image(ImTextureID(shadowBuffer2.getTextureID()), ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0), ImColor(255,255,255,255), ImColor(255,255,255,128));
    ImGui::Image(ImTextureID(shadowBuffer3.getTextureID()), ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0), ImColor(255,255,255,255), ImColor(255,255,255,128));
}
