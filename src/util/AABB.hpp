#pragma once

#include "Math.hpp"

class AABB {
public:
    vec3 min;
    vec3 max;

    AABB();

    /**
     * Creates a bounding cube from 2 vectors. The first is the minimum point of a cube
     * and the second is the maximum point. It actually doesn't matter to make sure min
     * is less than max, as the code automatically would fix it.
     */
    AABB(const vec3 &min, const vec3 &max);

    bool intersectsWith(const AABB &other) const;

    bool isVecInside(const vec3 &vec) const;

    /**
     * Tests if a ray intersects with this AABB.
     *
     * start is the starting position of the ray (e.g. the player's eye position.
     *
     * dir is a vector representing the direction of the ray.
     *
     * collisionPoint is a passthrough argument that is set to the point the ray touches this AABB.
     *
     * collisionNormal is a passthrough argument that is set to the normal of the face the ray touches.
     */
    bool raycast(const vec3 &start, const vec3 &dir, vec3 &collisionPoint, vec3 &collisionNormal) const;

    /**
     * If this AABB intersects the other AABB on the YZ axis, return the offset between them
     * on the X axis. If there is no intersection, returns provided offset.
     */
    float calculateXOffset(const AABB &other, float offset) const;

    /**
     * If this AABB intersects the other AABB on the XZ axis, return the offset between them
     * on the Y axis. If there is no intersection, returns provided offset.
     */
    float calculateYOffset(const AABB &other, float offset) const;

    /**
     * If this AABB intersects the other AABB on the XY axis, return the offset between them
     * on the Z axis. If there is no intersection, returns provided offset.
     */
    float calculateZOffset(const AABB &other, float offset) const;

    void move(const vec3 &offset);

    /**
     * Returns a point representing the specified corner of this AABB. Argument ranges from 0 to 7 for each of the
     * eight corners of a cube.
     */
    vec3 getCorner(int i) const;
};
