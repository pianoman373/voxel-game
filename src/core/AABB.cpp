#include "AABB.hpp"

#include <algorithm>

AABB::AABB() {
    this->min = vec3();
    this->max = vec3();
}

AABB::AABB(const vec3 &min, const vec3 &max) {
    this->min.x = std::min(min.x, max.x);
    this->min.y = std::min(min.y, max.y);
    this->min.z = std::min(min.z, max.z);

    this->max.x = std::max(min.x, max.x);
    this->max.y = std::max(min.y, max.y);
    this->max.z = std::max(min.z, max.z);
}

//Code stolen from Minecraft.
bool AABB::intersectsWith(const AABB &other) const
{
    return other.max.x > min.x && other.min.x < max.x ? (other.max.y > min.y && other.min.y < max.y ? (other.max.z > min.z && other.min.z < max.z) : false) : false;
}

//Code stolen from Minecraft.
bool AABB::isVecInside(const vec3 &vec) const
{
    return vec.x > min.x && vec.x < max.x ? (vec.y > min.y && vec.y < max.y ? (vec.z > min.z && vec.z < max.z) : false) : false;
}

//Code stolen from Minetest.
bool AABB::raycast(const vec3 &start, const vec3 &dir, vec3 &collisionPoint, vec3 &collisionNormal) const {
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
                    collisionNormal = vec3((dir.x > 0) ? -1.0f : 1.0f, 0.0f, 0.0f);
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
                    collisionNormal = vec3(0, (dir.y > 0) ? -1.0f : 1.0f, 0.0f);
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
                    collisionNormal = vec3(0, 0, (dir.z > 0) ? -1.0f : 1.0f);
                    return true;
                }
            }
        }
    return false;
}

//Code stolen from Minecraft.
float AABB::calculateXOffset(const AABB &other, float offset) const
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
float AABB::calculateYOffset(const AABB &other, float offset) const
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
float AABB::calculateZOffset(const AABB &other, float offset) const
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

void AABB::move(const vec3 &offset) {
    min = min + offset;
    max = max + offset;
}

vec3 AABB::getCorner(int i) const {
    switch (i) {
        case 0:
            return vec3(min.x, min.y, min.z);
        case 1:
            return vec3(min.x, min.y, max.z);
        case 2:
            return vec3(min.x, max.y, min.z);
        case 3:
            return vec3(min.x, max.y, max.z);
        case 4:
            return vec3(max.x, min.y, min.z);
        case 5:
            return vec3(max.x, min.y, max.z);
        case 6:
            return vec3(max.x, max.y, min.z);
        case 7:
            return vec3(max.x, max.y, max.z);
    }
    return vec3();
}
