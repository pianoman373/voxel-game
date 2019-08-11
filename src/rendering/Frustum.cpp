#include "rendering/Camera.hpp"
#include "util/AABB.hpp"
#include "rendering/Frustum.hpp"
#include "rendering/Renderer.hpp"

vec3 normFromPoints(vec3 v1, vec3 v2, vec3 v3) {
    vec3 aux1, aux2;

    aux1 = v1 - v2;
    aux2 = v3 - v2;

    return normalize(cross(aux2, aux1));
}

bool planeCollision(vec3 orig, vec3 norm, vec3 point) {
    return (dot(point - orig, norm) > 0.0f);
}

Frustum::Frustum() {

}

void Frustum::setupInternals(float fov, float aspect, float near, float far) {
    this->fov = fov;
    this->aspect = aspect;
    this->near = near;
    this->far = far;

    float tang = (float)tan(radians(fov) * 0.5) ;
    this->nh = near * tang;
    this->nw = nh * aspect;
    this->fh = far  * tang;
    this->fw = fh * aspect;
}

void Frustum::setupInternalsOrthographic(float left, float right, float bottom, float top, float near, float far) {
    this->near = near;
    this->far = far;

    this->nh = bottom - top;
    this->nw = right - left;
    this->fh = bottom - top;
    this->fw = right - left;
}

void Frustum::updateCamPosition(Camera &cam) {
    pos = cam.getPosition();
    dir = cam.getDirection();
    up = cam.getUp();
    right = cam.getRight();

    ntl = cam.getPosition();
    ntr = cam.getPosition();
    nbl = cam.getPosition();
    nbr = cam.getPosition();

    ntl = pos + ((dir * near) + (up * nh) - (right * nw));
    ntr = pos + ((dir * near) + (up * nh) + (right * nw));
    nbl = pos + ((dir * near) - (up * nh) - (right * nw));
    nbr = pos + ((dir * near) - (up * nh) + (right * nw));

    ftl = pos + ((dir * far) + (up * fh) - (right * fw));
    ftr = pos + ((dir * far) + (up * fh) + (right * fw));
    fbl = pos + ((dir * far) - (up * fh) - (right * fw));
    fbr = pos + ((dir * far) - (up * fh) + (right * fw));

    normalRight = normFromPoints(nbr, ntr, fbr);
    normalLeft = normFromPoints(ntl, nbl, fbl);
    normalTop = normFromPoints(ntr, ntl, ftr);
    normalBottom = normFromPoints(nbl, nbr, fbr);
}

void Frustum::renderDebug() const {
    vec3 color = vec3(1.0f, 0.0f, 0.0f);
    Renderer::debug.renderDebugLine(ftl, ntl, color);
    Renderer::debug.renderDebugLine(ftr, ntr, color);

    Renderer::debug.renderDebugLine(fbl, nbl, color);
    Renderer::debug.renderDebugLine(fbr, nbr, color);

    Renderer::debug.renderDebugLine(ftl, ftr, color);
    Renderer::debug.renderDebugLine(fbl, fbr, color);
    Renderer::debug.renderDebugLine(ftl, fbl, color);
    Renderer::debug.renderDebugLine(ftr, fbr, color);

    Renderer::debug.renderDebugLine(ntl, ntr, color);
    Renderer::debug.renderDebugLine(nbl, nbr, color);
    Renderer::debug.renderDebugLine(ntl, nbl, color);
    Renderer::debug.renderDebugLine(ntr, nbr, color);

    Renderer::debug.renderDebugLine(pos, pos + right, vec3(1.0f, 0.0f, 0.0f));
    Renderer::debug.renderDebugLine(pos, pos + up, vec3(0.0f, 1.0f, 0.0f));
    Renderer::debug.renderDebugLine(pos, pos + dir, vec3(0.0f, 0.0f, 1.0f));
}

bool Frustum::isBoxInside(const AABB &box) const {
    if (box.min == box.max)
        return true;

    vec3 sides[5] = {dir, normalRight, normalLeft, normalTop, normalBottom};
    vec3 offsets[5] = {pos + (dir * near), pos - (right * nw), pos + (right * nw), pos - (up * nh), pos + (up * nh)};

    //bool inside = false;

    for (int i = 0; i < 5; i++) {
        bool insideSide = false;


        for (int j = 0; j < 8; j++) {

            vec3 point = box.getCorner(j);

            if (planeCollision(offsets[i], sides[i], point)) {
                insideSide = true;
            }
        }

        if (!insideSide) {
            return false;
        }
    }

    return true;
}

bool Frustum::isPointInside(const vec3 &point) const {
    vec3 sides[5] = {dir, normalRight, normalLeft, normalTop, normalBottom};
    vec3 offsets[5] = {nbl, nbr, ntl, ntr, nbl};

    for (int i = 0; i < 5; i++) {
        if (!planeCollision(offsets[i], sides[i], point)) {
            return false;
        }
    }

    return true;
}
