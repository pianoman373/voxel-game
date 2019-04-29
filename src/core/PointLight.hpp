#pragma once

#include "Math.hpp"

class PointLight {
public:
    vec3 m_position;
    vec3 m_color;
    float m_radius;

    PointLight(vec3 position, vec3 color, float radius);
};