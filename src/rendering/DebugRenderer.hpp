#pragma once

#include "rendering/Mesh.hpp"
#include "util/AABB.hpp"
#include "rendering/Camera.hpp"
#include "rendering/Shader.hpp"

#include <vector>

class DebugRenderer {
private:
    Mesh debugRendererMesh;

    std::vector<vec3> positions;
    std::vector<vec3> colors;

public:
    /**
     * Render a line of the specified color from point v1 to v2
     */
    void renderDebugLine(const vec3 &v1, const vec3 &v2, const vec3 &color);

    /**
     * Draw a cube with specified color and min and max corners.
     */
    void renderDebugAABB(const vec3 &v1, const vec3 &v2, const vec3 &color);

    void renderDebugAABB(const AABB &aabb, const vec3 &color);

    void renderDebugSphere(const vec3 &pos, float radius, const vec3 &color);

    /**
     * Renders the accumulated debug calls into one big mesh. Generally the renderer will call this on it's own.
     */
    void flush(const Camera &cam);
};