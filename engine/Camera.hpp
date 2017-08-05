#pragma once

#include "Math.hpp"

class Camera {
public:
    vec3 position = vec3(0.0f, 0.0f, 0.0f);
    vec3 direction = vec3(0.0f, 0.0f, 1.0f);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);

public:
    vec3 getPosition();

    /**
     * Returns the forward "look" vector of the camera.
     */
    vec3 getDirection();

    /**
     * Returns the vector that is perpendicular to the look vector and the up vector (a.k.a "right").
     */
    vec3 getRight();

    vec3 getUp();

    void setPosition(const vec3 &position);

    /**
     * Sets the forward "look" vector of the camera.
     */
    void setDirection(const vec3 &direction);

    mat4 getView();

    mat4 getProjection();
};
