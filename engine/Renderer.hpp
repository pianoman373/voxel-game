#pragma once

#include "Math.hpp"
#include "Material.hpp"

class Mesh;
class AABB;
class Camera;
class Material;


struct RenderCall {
    Mesh *mesh;
    Material material;
    Transform transform;
};

class Renderer {

public:
    /**
     * Sets up vital shaders and variables only once at startup.
     */
    static void init(bool shadows, int shadowResolution);

    /**
     * General purpose abstraction of all render calls to an internal renderer.
     */
    static void render(Mesh *mesh, Material material, Transform transform);

    /**
     * Render a line of the specified color from point v1 to v2
     */
    static void renderDebugLine(vec3 v1, vec3 v2, vec3 color);

    /**
     * Draw a cube with specified color and min and max corners.
     */
    static void renderDebugAABB(vec3 v1, vec3 v2, vec3 color);

    /**
     * Draw a cube with specified color and AABB.
     */
    static void renderDebugAABB(AABB aabb, vec3 color);

    /**
     * In order to allow render to be called from anywhere at any time, render calls
     * are put in a buffer to be drawn later. Calling flush renders them all at once and
     * clears the buffer for the next time.
     */
    static void flush(Camera cam);
};
