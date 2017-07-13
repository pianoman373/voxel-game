#pragma once

#include "Math.hpp"

class AABB {
public:
    vec3 min;
    vec3 max;

    /**
     * Creates a bounding cube from 2 vectors. The first is the minimum point of a cube
     * and the second is the maximum point. It actually doesn't matter to make sure min
     * is less than max, as the code automatically would fix it.
     */
    AABB(vec3 min, vec3 max);

    /**
     * Returns true if this AABB intersects with another.
     */
    bool intersectsWith(const AABB &other);

    /**
     * Returns true if the specified vector is inside this AABB.
     */
    bool isVecInside(vec3 vec);

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
    bool raycast(vec3 start, vec3 dir, vec3 &collisionPoint, vec3 &collisionNormal);

    /**
     * If this AABB intersects the other AABB on the YZ axis, return the offset between them
     * on the X axis. If there is no intersection, returns provided offset.
     */
    float calculateXOffset(AABB &other, float offset);

    /**
     * If this AABB intersects the other AABB on the XZ axis, return the offset between them
     * on the Y axis. If there is no intersection, returns provided offset.
     */
    float calculateYOffset(AABB &other, float offset);

    /**
     * If this AABB intersects the other AABB on the XY axis, return the offset between them
     * on the Z axis. If there is no intersection, returns provided offset.
     */
    float calculateZOffset(AABB &other, float offset);

    void move(vec3 dir);

    vec3 getCorner(int i);
};
