#include "rendering/Renderer.hpp"
#include "rendering/Framebuffer.hpp"
#include "client/Window.hpp"
#include "rendering/Camera.hpp"
#include "util/AABB.hpp"
#include "rendering/Frustum.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/Primitives.hpp"
#include "Math.hpp"
#include "util/Input.hpp"
#include "rendering/IBL.hpp"
#include "rendering/DebugRenderer.hpp"
#include "rendering/Resources.hpp"
#include "optick.h"

#include <glad/glad.h>
#include <imgui.h>
#include <deque>

static std::vector<RenderCall> renderQueue;
static std::vector<RenderCall> renderQueueForward;
static std::vector<PointLight*> pointLights;
static std::vector<DirectionalLight*> directionalLights;

static Framebuffer gBuffer;

static vec2i resolution;

static vec3 clearColor;

static GLuint queries[4]; // The unique query id
static GLuint queryResults[4]; // Save the time, in nanoseconds

static const bool doQueries = true;

static void beginQuery(int id) {
    if (doQueries) {
        glGetQueryObjectuiv(queries[id], GL_QUERY_RESULT, &queryResults[id]);
        glBeginQuery(GL_TIME_ELAPSED, queries[id]);
    }
}

static void endQuery() {
    if (doQueries) {
        glEndQuery(GL_TIME_ELAPSED);
    }
}

static void iterateCommandBuffer(std::vector<RenderCall> &buffer, const Camera &cam, const Frustum &f, bool doFrustumCulling) {
    OPTICK_EVENT();
    const Material *lastMaterial = nullptr;

    for (RenderCall &call : buffer) {
        //OPTICK_EVENT("command buffer iteration");
        if (doFrustumCulling) {
            if (!f.isBoxInside(call.aabb)) {
                continue;
            }
        }

        Shader s = call.material->getShader();

        if (call.material != lastMaterial) {
            OPTICK_EVENT("material rebinding");
            s.bind();
            call.material->bindUniforms();

            s.uniformVec3("cameraPos", cam.position);
            s.uniformMat4("view", cam.getView());
            s.uniformMat4("projection", cam.getProjection());
            

            if (call.material->needsGbuffer) {
                gBuffer.getAttachment(0).bind(0);
                gBuffer.getAttachment(1).bind(1);
                gBuffer.getAttachment(2).bind(2);
                gBuffer.getAttachment(3).bind(3);

                s.uniformInt("gPosition", 0);
                s.uniformInt("gNormal", 1);
                s.uniformInt("gAlbedo", 2);
                s.uniformInt("gRoughnessMetallic", 3);
            }
        }
        s.uniformMat4("model", call.transform);
        
        call.mesh->render();
        

        lastMaterial = call.material;
    }
}

static void iterateCommandBufferDepthOnly(std::vector<RenderCall> &buffer, const Camera &cam, const Frustum &f, bool doFrustumCulling) {
    Resources::ShadowShader.bind();

    Resources::ShadowShader.uniformMat4("view", cam.getView());
    Resources::ShadowShader.uniformMat4("projection", cam.getProjection());
    
    for (RenderCall c : buffer) {
        if (doFrustumCulling) {
            if (!f.isBoxInside(c.aabb)) {
                continue;
            }
        }

        Resources::ShadowShader.uniformMat4("model", c.transform);

        c.mesh->render();
    }
}

namespace Renderer {
    DebugRenderer debug;

    Cubemap irradiance;
    Cubemap specular;

    
    Framebuffer HDRbuffer;
    Framebuffer HDRbuffer2;

    std::vector<std::shared_ptr<PostProcessor>> postProcessingStack;

    void init(int resolutionX, int resolutionY) {
        resolution = vec2i(resolutionX, resolutionY);

        Resources::loadDefaultResources();

        glGenQueries(4, queries);

        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glEnable(GL_DEPTH_TEST);

        glLineWidth(2.0f);

        resize(resolution.x, resolution.y);
    }

    void resize(int resolutionX, int resolutionY) {
        resolution = vec2i(resolutionX, resolutionY);

        HDRbuffer.destroy();
        HDRbuffer.setup(resolution.x, resolution.y);
        HDRbuffer.attachTexture(GL_RGB16F, GL_RGB, GL_FLOAT);
        HDRbuffer.attachRBO();

        HDRbuffer2.destroy();
        HDRbuffer2.setup(resolution.x, resolution.y);
        HDRbuffer2.attachTexture(GL_RGB16F, GL_RGB, GL_FLOAT);
        HDRbuffer2.attachRBO();

        gBuffer.destroy();
        gBuffer.setup(resolution.x, resolution.y);
        gBuffer.attachTexture(GL_RGB16F, GL_RGB, GL_FLOAT); //position
        gBuffer.attachTexture(GL_RGB16F, GL_RGB, GL_FLOAT); //normal
        gBuffer.attachTexture(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE); //color + specular
        gBuffer.attachTexture(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE); //roughness + metallic + 2 extra channels
        gBuffer.attachRBO();

        for (size_t i = 0; i < postProcessingStack.size(); i++) {
            postProcessingStack[i]->resize();
        }
    }

    void matchWindowResolution(float scale) {
        if (!(resolution == vec2i(Window::getWindowSize().x * scale, Window::getWindowSize().y * scale))) {
            resize(Window::getWindowSize().x * scale, Window::getWindowSize().y * scale);
        }
    }

    void renderPointLight(PointLight *light) {
        pointLights.push_back(light);
    }

    void renderDirectionalLight(DirectionalLight *light) {
        directionalLights.push_back(light);
    }

    void render(const IRenderable *mesh, const Material *material, mat4 transform, AABB aabb) {
        RenderCall call;
        call.mesh = mesh;
        call.material = material;
        call.transform = transform;
        call.aabb = aabb;

        if (material->deferred) {
            renderQueue.push_back(call);
        }
        else {
            renderQueueForward.push_back(call);
        }
    }

    void renderSkybox(const Material *material) {
        render(&Resources::cubemapMesh, material, mat4(), AABB());
    }

    void renderToDepth(const Framebuffer &target, const Camera &cam, const Frustum &f, bool doFrustumCulling) {
        glViewport(0, 0, target.getWidth(), target.getHeight());
        target.bind();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);

        iterateCommandBufferDepthOnly(renderQueue, cam, f, doFrustumCulling);        
    }

    void renderToFramebuffer(const Camera &cam, const Frustum &f, bool doFrustumCulling) {
        OPTICK_EVENT();
        glDisable(GL_BLEND);

        beginQuery(0);
        // render objects in scene into g-buffer
        // -------------------------------------
        gBuffer.bind();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, resolution.x, resolution.y);

        iterateCommandBuffer(renderQueue, cam, f, doFrustumCulling);
        endQuery();
        
        // apply lighting to g-buffers
        // -------------------------------
        beginQuery(1);

        for (int i = 0; i < directionalLights.size(); i++) {
            directionalLights[i]->preRender(cam);
        }
        

        endQuery();
        beginQuery(2);

        HDRbuffer.bind();
        glViewport(0, 0, resolution.x, resolution.y);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glDepthFunc(GL_ALWAYS);
        glDepthMask(GL_TRUE);

        gBuffer.getAttachment(0).bind(0);
        gBuffer.getAttachment(1).bind(1);
        gBuffer.getAttachment(2).bind(2);
        gBuffer.getAttachment(3).bind(3);

        mat4 inverseView = inverse(cam.getView());

        // light the g-buffers with the deferred shader
        // ---------------------------------------------
        Resources::deferredShader.bind();

        Resources::deferredShader.uniformInt("gPosition", 0);
        Resources::deferredShader.uniformInt("gNormal", 1);
        Resources::deferredShader.uniformInt("gAlbedo", 2);
        Resources::deferredShader.uniformInt("gRoughnessMetallic", 3);
        Resources::deferredShader.uniformMat4("view", cam.getView());

        Resources::framebufferMesh.render();

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        // render directional lighting to the buffer
        // ---------------------------------------------
        for (int i = 0; i < directionalLights.size(); i++) {
            directionalLights[i]->render(cam);
        }

        // Render ambient lighting to the buffer
        // ---------------------------------------------
        if (irradiance.getID() != 0 && specular.getID() != 0) {
            Resources::deferredAmbientShader.bind();

            Resources::deferredAmbientShader.uniformInt("gPosition", 0);
            Resources::deferredAmbientShader.uniformInt("gNormal", 1);
            Resources::deferredAmbientShader.uniformInt("gAlbedo", 2);
            Resources::deferredAmbientShader.uniformInt("gRoughnessMetallic", 3);
            

            Resources::deferredAmbientShader.uniformInt("irradiance", 4);
            irradiance.bind(4);
            Resources::deferredAmbientShader.uniformInt("prefilter", 5);
            specular.bind(5);
            Resources::deferredAmbientShader.uniformInt("brdf", 6);
            Resources::brdf.bind(6);

            Resources::deferredAmbientShader.uniformMat4("inverseView", inverseView);

            Resources::framebufferMesh.render();
        }
        

        endQuery();

        // copy depth and stencil buffer
        // -------------------------------
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, HDRbuffer.fbo);
        glBlitFramebuffer(0, 0, resolution.x, resolution.y, 0, 0, resolution.x, resolution.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBlitFramebuffer(0, 0, resolution.x, resolution.y, 0, 0, resolution.x, resolution.y, GL_STENCIL_BUFFER_BIT, GL_NEAREST);


        // render the forward pass
        // -------------------------------
        //glDisable(GL_CULL_FACE);

        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        iterateCommandBuffer(renderQueueForward, cam, f, doFrustumCulling);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ///glEnable(GL_CULL_FACE);

        debug.flush(cam);
    }

    void clear() {
        pointLights.clear();
        directionalLights.clear();
        renderQueue.clear();
        renderQueueForward.clear();
    }

    void flush(const Camera &cam) {
        OPTICK_EVENT();
        Frustum f;
        flush(cam, f, false);
    }

    void flush(const Camera &cam, const Frustum &f, bool doFrustumCulling) {
        OPTICK_EVENT();
        const Texture &t = flushToTexture(cam, f, doFrustumCulling);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Window::getWindowSize().x, Window::getWindowSize().y);
        Resources::gammaCorrectShader.bind();
        t.bind();
        Resources::framebufferMesh.render();
    }

    const Texture &flushToTexture(const Camera &cam) {
        OPTICK_EVENT();
        Frustum f;
        return flushToTexture(cam, f, false);
    }

    const Texture &flushToTexture(const Camera &cam, const Frustum &f, bool doFrustumCulling) {
        OPTICK_EVENT();
        renderToFramebuffer(cam, f, doFrustumCulling);
        
        glDepthFunc(GL_ALWAYS);
        // post processing
        beginQuery(3);
        Framebuffer *source = &HDRbuffer;
        Framebuffer *destination = &HDRbuffer2;

        if (postProcessingStack.size() > 0) {
            for (size_t i = 0; i < postProcessingStack.size(); i++) {
                std::shared_ptr<PostProcessor> step = postProcessingStack[i];

                step->postProcess(cam, *source, *destination);

                if (i != postProcessingStack.size()-1) {
                    Framebuffer *temp = destination;
                    destination = source;
                    source = temp;
                }
            }
        }
        else {
            destination = &HDRbuffer;
        }
        endQuery();

        glDepthFunc(GL_LEQUAL);

        static bool lastKeydown = false;
        static bool debug = false;
        if (Input::isKeyDown(Input::KEY_F1) && !lastKeydown) {
            debug = !debug;
        }
        lastKeydown = Input::isKeyDown(Input::KEY_F1);

        

        if (debug) {
            static std::vector<float> frameTimes(60*2, 3.0f);

            bool p_open = false;
            if (ImGui::Begin("Example: Fixed Overlay", &p_open, ImVec2(0, 0), 0.3f,
                            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoSavedSettings)) {
                            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", Window::deltaTime() * 1000.0f,
                            ImGui::GetIO().Framerate);

                            ImGui::Text("geometry: %.2f ms", queryResults[0]*0.000001f);
                            ImGui::Text("shadow pass: %.2f ms", queryResults[1]*0.000001f);
                            ImGui::Text("deferred lighting: %.2f ms", queryResults[2]*0.000001f);
                            ImGui::Text("post processing: %.2f ms", queryResults[3]*0.000001f);

                            float average = 0.0f;

                            for (int i = 0; i < frameTimes.size() - 1; i++) {
                                frameTimes[i] = frameTimes[i+1];
                                average += frameTimes[i];
                            }
                            average /= frameTimes.size();
                            frameTimes[frameTimes.size()-1] = Window::deltaTime() * 1000.0f;
                            std::string message = "average " + std::to_string(average) + "ms";

                            ImGui::PlotLines("##plot", frameTimes.data(), frameTimes.size(), 0, message.data(), 0.0f, 40.0f, ImVec2(400,200));
                            

                            


                ImGui::End();

            }

            ImGui::ShowDemoWindow();
        }

        pointLights.clear();
        directionalLights.clear();
        renderQueue.clear();
        renderQueueForward.clear();

        return destination->getAttachment(0);
    }

    Cubemap renderToProbe(const vec3 &position) {
        static const int resolution = 128;

        static vec3 forwards[] = {
                vec3(1.0f,  0.0f,  0.0f),
                vec3(-1.0f,  0.0f,  0.0f),
                vec3(0.0f,  1.0f,  0.0f),
                vec3(0.0f, -1.0f,  0.0f),
                vec3(0.0f,  0.0f,  1.0f),
                vec3(0.0f,  0.0f, -1.0f)
        };

        static vec3 ups[] = {
                vec3(0.0f,  -1.0f,  0.0f),
                vec3(0.0f,  -1.0f,  0.0f),
                vec3(0.0f,  0.0f,  1.0f),
                vec3(0.0f, 0.0f,  -1.0f),
                vec3(0.0f,  -1.0f, 0.0f),
                vec3(0.0f,  -1.0f, 0.0f)
        };

        unsigned int captureFBO, captureRBO;
        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

        unsigned int envCubemap;

        glGenTextures(1, &envCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        for (unsigned int i = 0; i < 6; ++i)
        {
            // note that we store each face with 16 bit floating point values
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                         resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        vec2i internalResolution = getResolution();
        resize(resolution, resolution);

        glViewport(0, 0, resolution, resolution);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        for (unsigned int i = 0; i < 6; ++i)
        {
            Texture gPosition;
            Texture gNormal;
            Texture gAlbedo;
            Texture gRoughnessMetallic;

            Camera cam;
            cam.dimensions = {(float)resolution, (float)resolution};
            cam.position = position;

            cam.direction = forwards[i];
            cam.up = ups[i];
            cam.fov = 90.0f;

            renderToFramebuffer(cam, Frustum(), false);

            glViewport(0, 0, resolution, resolution);
            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
            glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Resources::passthroughShader.bind();
            HDRbuffer.getAttachment(0).bind();
            Resources::framebufferMesh.render();
        }

        resize(internalResolution.x, internalResolution.y);

        glDeleteFramebuffers(1, &captureFBO);
        glDeleteRenderbuffers(1, &captureRBO);

        Cubemap c;
        c.setID(envCubemap);

        return c;
    }

    void setClearColor(vec3 color) {
        clearColor = color;
    }

    vec2i getResolution() {
        return resolution;
    }

    Framebuffer &getGBuffer() {
        return gBuffer;
    }
}
