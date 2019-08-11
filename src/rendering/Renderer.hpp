#pragma once

#include "Math.hpp"
#include "rendering/Material.hpp"
#include "util/AABB.hpp"
#include "rendering/Camera.hpp"
#include "rendering/DebugRenderer.hpp"
#include "rendering/Frustum.hpp"
#include "rendering/IRenderable.hpp"
#include "rendering/PostProcessing.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/DirectionalLight.hpp"
#include "rendering/PointLight.hpp"

#include <vector>


struct RenderCall {
	const IRenderable *mesh;
	const Material *material;
	mat4 transform;
	AABB aabb;
};

namespace Renderer {
	extern DebugRenderer debug;

	extern Cubemap irradiance;
	extern Cubemap specular;

    extern Framebuffer HDRbuffer;
    extern Framebuffer HDRbuffer1;

    extern std::vector<std::shared_ptr<PostProcessor>> postProcessingStack;

    /**
     * Sets up vital shaders and variables only once at startup.
     */
    void init(int resolutionX, int resolutionY);

    void resize(int resolutionX, int resolutionY);

    void matchWindowResolution(float scale=1.0f);

	/**
	 * Pushes a point light to the render buffer for the next flush event.
	 */
    void renderPointLight(PointLight *light);

    void renderDirectionalLight(DirectionalLight *light);

    /**
     * General purpose abstraction of all render calls to an internal renderer.
     */
    void render(const IRenderable *mesh, const Material *material, mat4 transform, AABB aabb);

    void renderSkybox(const Material *material);

    void renderToDepth(const Framebuffer &target, const Camera &cam, const Frustum &f, bool doFrustumCulling);

	Cubemap renderToProbe(const vec3 &position);

    void renderToFramebuffer(const Camera &cam, const Frustum &f, bool doFrustumCulling);

    void clear();

	/**
	* Flush command with frustum culling disabled.
	*/
	void flush(const Camera &cam);

	/**
     * In order to allow render to be called from anywhere at any time, render calls
     * are put in a buffer to be drawn later. Calling flush renders them all at once and
     * clears the buffer for the next time.
     */
    void flush(const Camera &cam, const Frustum &f, bool doFrustumCulling = true);

    const Texture &flushToTexture(const Camera &cam);

	const Texture &flushToTexture(const Camera &cam, const Frustum &f, bool doFrustumCulling = true);

    void renderToFramebuffer(const Camera &cam, const Frustum &f, bool doFrustumCulling = true);

    void setClearColor(vec3 color);

    vec2i getResolution();

    Framebuffer &getGBuffer();
};
