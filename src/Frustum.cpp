#include "Frustum.hpp"
#include "Renderer.hpp"

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

    float tang = (float)tan(Deg2Rad(fov) * 0.5) ;
    this->nh = near * tang;
    this->nw = nh * aspect;
    this->fh = far  * tang;
    this->fw = fh * aspect;
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

void Frustum::renderDebug() {
    vec3 color = vec3(1.0f, 0.0f, 0.0f);
    Renderer::renderDebugLine(ftl, ntl, color);
    Renderer::renderDebugLine(ftr, ntr, color);

    Renderer::renderDebugLine(fbl, nbl, color);
    Renderer::renderDebugLine(fbr, nbr, color);

    Renderer::renderDebugLine(ftl, ftr, color);
    Renderer::renderDebugLine(fbl, fbr, color);
    Renderer::renderDebugLine(ftl, fbl, color);
    Renderer::renderDebugLine(ftr, fbr, color);

    Renderer::renderDebugLine(ntl, ntr, color);
    Renderer::renderDebugLine(nbl, nbr, color);
    Renderer::renderDebugLine(ntl, nbl, color);
    Renderer::renderDebugLine(ntr, nbr, color);

    Renderer::renderDebugLine(pos, pos + right, vec3(1.0f, 0.0f, 0.0f));
    Renderer::renderDebugLine(pos, pos + up, vec3(0.0f, 1.0f, 0.0f));
    Renderer::renderDebugLine(pos, pos + dir, vec3(0.0f, 0.0f, 1.0f));
}

bool Frustum::isBoxInside(AABB box) {
    vec3 sides[5] = {dir, normalRight, normalLeft, normalTop, normalBottom};

    //bool inside = false;

    for (int i = 0; i < 5; i++) {
        bool insideSide = false;


        for (int j = 0; j < 8; j++) {

            vec3 point = box.getCorner(j);

            if (planeCollision(pos, sides[i], point)) {
                insideSide = true;
            }
        }

        if (!insideSide) {
            return false;
        }
    }

    return true;
}

bool Frustum::isPointInside(vec3 point) {
    vec3 sides[5] = {dir, normalRight, normalLeft, normalTop, normalBottom};

    for (int i = 0; i < 5; i++) {
        if (!planeCollision(pos, sides[i], point)) {
            return false;
        }
    }

    return true;
}
