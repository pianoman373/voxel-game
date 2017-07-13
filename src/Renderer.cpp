#include "Renderer.hpp"
#include "Framebuffer.hpp"
#include "GL/glew.h"
#include "MeshFactory.hpp"
#include "Settings.hpp"
#include "Client.hpp"

#include <imgui.h>
#include <stack>

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
Shader skyboxShader;

Mesh skyboxMesh;
Mesh crosshairMesh;

MeshFactory ms;
Mesh mesh;

static void renderShadow(Framebuffer &fbuffer, mat4 lightSpaceMatrix) {
    glViewport(0, 0, fbuffer.getWidth(), fbuffer.getHeight());
    fbuffer.bind();
    glClear(GL_DEPTH_BUFFER_BIT);

    std::stack<RenderCall> renderQueueCopy = renderQueue;

    for (unsigned int i = 0; i < renderQueueCopy.size(); i++) {
        RenderCall c = renderQueueCopy.top();
        renderQueueCopy.pop();

        ShadowShader.bind();
        ShadowShader.uniformMat4("lightSpaceMatrix", lightSpaceMatrix);

        mat4 model;
        model = translate(model, c.transform.position);
        ShadowShader.uniformMat4("model", model);

        c.mesh->render();
    }
}

static mat4 shadowMatrix(float radius, Camera &cam) {
    return orthographic(-radius, radius, -radius, radius, -radius * 8, radius * 8) * LookAt(cam.getPosition() + sunDirection, cam.getPosition(), vec3( 0.0f, 1.0f,  0.0f));
}

void Renderer::init() {
    if (Settings::shadows) {
        shadowBuffer0.setupShadow(Settings::shadow_resolution, Settings::shadow_resolution);
        shadowBuffer1.setupShadow(Settings::shadow_resolution, Settings::shadow_resolution);
        shadowBuffer2.setupShadow(Settings::shadow_resolution, Settings::shadow_resolution);
        shadowBuffer3.setupShadow(Settings::shadow_resolution, Settings::shadow_resolution);
        ShadowShader.load("resources/shadow.vsh", "resources/shadow.fsh");
    }

    debugShader.load("resources/debug.vsh", "resources/debug.fsh");
    skyboxShader.load("resources/skybox.vsh", "resources/skybox.fsh");


    MeshFactory m;
    m.vertex(1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex(1.0f,  1.0f,  -1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex(1.0f,  -1.0f,  -1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);

    m.vertex(1.0f,  -1.0f,  -1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex(1.0f,  -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex(1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);


    m.vertex( -1.0f,  -1.0f,  -1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( -1.0f,  1.0f,  -1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( -1.0f,  1.0f,  1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);

    m.vertex( -1.0f,  1.0f,  1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( -1.0f,  -1.0f,  1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( -1.0f,  -1.0f,  -1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);


    m.vertex( -1.0f,  1.0f,  -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( 1.0f,  1.0f,  -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( 1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);

    m.vertex( 1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( -1.0f,  1.0f,  -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);


    m.vertex( 1.0f,  -1.0f,  1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( 1.0f,  -1.0f,  -1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( -1.0f, -1.0f,  -1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);

    m.vertex( -1.0f,  -1.0f,  -1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( -1.0f,  -1.0f,  1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( 1.0f,  -1.0f,  1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);


    m.vertex( -1.0f,  -1.0f,  -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( 1.0f,  -1.0f,  -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( 1.0f,  1.0f,  -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);

    m.vertex( 1.0f,  1.0f,  -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( -1.0f,  1.0f,  -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( -1.0f,  -1.0f,  -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);


    m.vertex( 1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( 1.0f,  -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( -1.0f,  -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);

    m.vertex( -1.0f,  -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( 1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);

    m.toMesh(skyboxMesh);
    m.clear();

    m.vertex( 10.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( 10.0f,  -1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( -10.0f,  -1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);

    m.vertex( -10.0f,  -1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( -10.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( 10.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);

    m.vertex( 1.0f,  10.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( 1.0f,  -10.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( -1.0f,  -10.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);

    m.vertex( -1.0f,  -10.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( -1.0f,  10.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);
    m.vertex( 1.0f,  10.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f);

    m.toMesh(crosshairMesh);
}

void Renderer::render(Mesh *mesh, Material material, Transform transform) {
    RenderCall call;
    call.mesh = mesh;
    call.material = material;
    call.transform = transform;

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

    //shadow buffer
    if (Settings::shadows) {
        renderShadow(shadowBuffer0, lightSpaceMatrix0);
        renderShadow(shadowBuffer1, lightSpaceMatrix1);
        renderShadow(shadowBuffer2, lightSpaceMatrix2);
        renderShadow(shadowBuffer3, lightSpaceMatrix3);
    }

    //normal rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    vec2i size = Client::window.getWindowSize();
    glViewport(0, 0, size.x, size.y);

    glDepthMask(false);
    skyboxShader.bind();

    mat4 skyboxModel;
    skyboxModel = translate(skyboxModel, cam.getPosition());
    skyboxShader.uniformMat4("model", skyboxModel);
    skyboxShader.uniformMat4("view", cam.getView());
    skyboxShader.uniformMat4("projection", cam.getProjection());
    skyboxShader.uniformVec3("sunDirection", sunDirection);
    skyboxShader.uniformVec3("sunColor", sunColor);

    skyboxMesh.render();
    glDepthMask(true);



    for (unsigned int i = 0; i < renderQueue.size(); i++) {
            RenderCall call = renderQueue.top();
            renderQueue.pop();

            Shader s = call.material.getShader();
            s.bind();
            call.material.bindUniforms();

            if (Settings::shadows) {
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

            mat4 model;
            model = translate(model, call.transform.position);
            s.uniformMat4("model", model);

            call.mesh->render();
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


//    ImGui::Image(ImTextureID(shadowBuffer0.getTextureID()), ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0), ImColor(255,255,255,255), ImColor(255,255,255,128));
//    ImGui::Image(ImTextureID(shadowBuffer1.getTextureID()), ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0), ImColor(255,255,255,255), ImColor(255,255,255,128));
//    ImGui::Image(ImTextureID(shadowBuffer2.getTextureID()), ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0), ImColor(255,255,255,255), ImColor(255,255,255,128));
//    ImGui::Image(ImTextureID(shadowBuffer3.getTextureID()), ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0), ImColor(255,255,255,255), ImColor(255,255,255,128));
}
