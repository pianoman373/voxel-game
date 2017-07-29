#pragma once

#include "Math.hpp"

class Camera;
class AABB;

class Frustum {
public:

    vec3 ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
    float fov, aspect, near, far;
    float nw, nh, fw, fh;

    vec3 pos, dir, up, right;
    vec3 normalTop, normalBottom, normalLeft, normalRight;

    Frustum();

    void setupInternals(float fov, float aspect, float near, float far);

    void setupInternalsOrthographic(float left, float right, float bottom, float top, float near, float far);

    void updateCamPosition(Camera &cam);

    void renderDebug();

    bool isPointInside(vec3 point);

    bool isBoxInside(AABB box);
};
