#include "AABB.hpp"

AABB::AABB(vec3 min, vec3 max) {
    this->min.x = std::min(min.x, max.x);
    this->min.y = std::min(min.y, max.y);
    this->min.z = std::min(min.z, max.z);

    this->max.x = std::max(min.x, max.x);
    this->max.y = std::max(min.y, max.y);
    this->max.z = std::max(min.z, max.z);
}

//Code stolen from Minecraft.
bool AABB::intersectsWith(const AABB &other)
{
    return other.max.x > min.x && other.min.x < max.x ? (other.max.y > min.y && other.min.y < max.y ? (other.max.z > min.z && other.min.z < max.z) : false) : false;
}

//Code stolen from Minecraft.
bool AABB::isVecInside(vec3 vec)
{
    return vec.x > min.x && vec.x < max.x ? (vec.y > min.y && vec.y < max.y ? (vec.z > min.z && vec.z < max.z) : false) : false;
}

//Code stolen from Minetest.
bool AABB::raycast(vec3 start, vec3 dir, vec3 &collisionPoint, vec3 &collisionNormal) {
    if (isVecInside(start)) {
            collisionPoint = start;
            collisionNormal = vec3();
            return true;
        }
        float m = 0;

        // Test X collision
        if (dir.x != 0) {
            if (dir.x > 0)
                m = (min.x - start.x) / dir.x;
            else
                m = (max.x - start.x) / dir.x;

            if (m >= 0 && m <= 1) {
                collisionPoint = start + dir * m;
                if ((collisionPoint.y >= min.y)
                        && (collisionPoint.y <= max.y)
                        && (collisionPoint.z >= min.z)
                        && (collisionPoint.z <= max.z)) {
                    collisionNormal = vec3((dir.x > 0) ? -1 : 1, 0, 0);
                    return true;
                }
            }
        }

        // Test Y collision
        if (dir.y != 0) {
            if (dir.y > 0)
                m = (min.y - start.y) / dir.y;
            else
                m = (max.y - start.y) / dir.y;

            if (m >= 0 && m <= 1) {
                collisionPoint = start + dir * m;
                if ((collisionPoint.x >= min.x)
                        && (collisionPoint.x <= max.x)
                        && (collisionPoint.z >= min.z)
                        && (collisionPoint.z <= max.z)) {
                    collisionNormal = vec3(0, (dir.y > 0) ? -1 : 1, 0);
                    return true;
                }
            }
        }

        // Test Z collision
        if (dir.z != 0) {
            if (dir.z > 0)
                m = (min.z - start.z) / dir.z;
            else
                m = (max.z - start.z) / dir.z;

            if (m >= 0 && m <= 1) {
                collisionPoint = start + dir * m;
                if ((collisionPoint.x >= min.x)
                        && (collisionPoint.x <= max.x)
                        && (collisionPoint.y >= min.y)
                        && (collisionPoint.y <= max.y)) {
                    collisionNormal = vec3(0, 0, (dir.z > 0) ? -1 : 1);
                    return true;
                }
            }
        }
    return false;
}

//Code stolen from Minecraft.
float AABB::calculateXOffset(AABB &other, float offset)
{
    if (other.max.y > min.y && other.min.y < max.y && other.max.z > min.z && other.min.z < max.z)
    {
        float d1;

        if (offset > 0.0f && other.max.x <= min.x)
        {
            d1 = min.x - other.max.x;

            if (d1 < offset)
            {
                offset = d1;
            }
        }
        else if (offset < 0.0f && other.min.x >= max.x)
        {
            d1 = max.x - other.min.x;

            if (d1 > offset)
            {
                offset = d1;
            }
        }

        return offset;
    }
    else
    {
        return offset;
    }
}

//Code stolen from Minecraft.
float AABB::calculateYOffset(AABB &other, float offset)
{
    if (other.max.x > min.x && other.min.x < max.x && other.max.z > min.z && other.min.z < max.z)
    {
        float d1;

        if (offset > 0.0f && other.max.y <= min.y)
        {
            d1 = min.y - other.max.y;

            if (d1 < offset)
            {
                offset = d1;
            }
        }
        else if (offset < 0.0f && other.min.y >= max.y)
        {
            d1 = max.y - other.min.y;

            if (d1 > offset)
            {
                offset = d1;
            }
        }

        return offset;
    }
    else
    {
        return offset;
    }
}

//Code stolen from Minecraft.
float AABB::calculateZOffset(AABB &other, float offset)
{
    if (other.max.x > min.x && other.min.x < max.x && other.max.y > min.y && other.min.y < max.y)
    {
        float d1;

        if (offset > 0.0f && other.max.z <= min.z)
        {
            d1 = min.z - other.max.z;

            if (d1 < offset)
            {
                offset = d1;
            }
        }
        else if (offset < 0.0f && other.min.z >= max.z)
        {
            d1 = max.z - other.min.z;

            if (d1 > offset)
            {
                offset = d1;
            }
        }

        return offset;
    }
    else
    {
        return offset;
    }
}

void AABB::move(vec3 dir) {
    min = min + dir;
    max = max + dir;
}
