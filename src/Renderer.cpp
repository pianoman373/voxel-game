#include "Renderer.hpp"
#include "Framebuffer.hpp"
#include "GL/glew.h"
#include "MeshFactory.hpp"

#include <imgui.h>

#define SHADOW_RESOLUTION 2048

const float cascadeDistances[4] = {10.0f, 40.0f, 100.0f, 500.0f};

std::vector<RenderCall> renderQueue;

Framebuffer shadowBuffer0;
Framebuffer shadowBuffer1;
Framebuffer shadowBuffer2;
Framebuffer shadowBuffer3;

Shader ShadowShader;
Shader debugShader;

MeshFactory ms;

static void renderShadow(Framebuffer &fbuffer, mat4 lightSpaceMatrix) {
    glViewport(0, 0, fbuffer.getWidth(), fbuffer.getHeight());
    fbuffer.bind();
    glClear(GL_DEPTH_BUFFER_BIT);

    for (unsigned int i = 0; i < renderQueue.size(); i++) {
        RenderCall c = renderQueue[i];

        ShadowShader.bind();
        ShadowShader.uniformMat4("lightSpaceMatrix", lightSpaceMatrix);

        mat4 model;
        model = translate(model, c.transform.position);
        ShadowShader.uniformMat4("model", model);

        c.mesh->render();
    }
}

static mat4 shadowMatrix(float radius, Camera &cam) {
    return orthographic(-radius, radius, -radius, radius, -radius * 8, radius * 8) * LookAt(cam.getPosition() + vec3(0.9, 1.1, 1.0), cam.getPosition(), vec3( 0.0f, 1.0f,  0.0f));
}

void Renderer::init() {
    shadowBuffer0.setupShadow(SHADOW_RESOLUTION, SHADOW_RESOLUTION);
    shadowBuffer1.setupShadow(SHADOW_RESOLUTION, SHADOW_RESOLUTION);
    shadowBuffer2.setupShadow(SHADOW_RESOLUTION, SHADOW_RESOLUTION);
    shadowBuffer3.setupShadow(SHADOW_RESOLUTION, SHADOW_RESOLUTION);

    ShadowShader.load("resources/shadow.vsh", "resources/shadow.fsh");
    debugShader.load("resources/debug.vsh", "resources/debug.fsh");

}

void Renderer::render(Mesh *mesh, Material material, Transform transform) {
    RenderCall call;
    call.mesh = mesh;
    call.material = material;
    call.transform = transform;

    renderQueue.push_back(call);
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
    renderShadow(shadowBuffer0, lightSpaceMatrix0);
    renderShadow(shadowBuffer1, lightSpaceMatrix1);
    renderShadow(shadowBuffer2, lightSpaceMatrix2);
    renderShadow(shadowBuffer3, lightSpaceMatrix3);

    //normal rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1400, 800);


    for (unsigned int i = 0; i < renderQueue.size(); i++) {
            RenderCall call = renderQueue[i];

            Shader s = call.material.getShader();
            s.bind();
            call.material.bindUniforms();

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

            s.uniformMat4("view", cam.getView());
            s.uniformMat4("projection", cam.getProjection());
            s.uniformVec3("cameraPos", cam.getPosition());

            mat4 model;
            model = translate(model, call.transform.position);
            s.uniformMat4("model", model);

            call.mesh->render();
    }

    renderQueue.clear();

    Mesh mesh;
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
