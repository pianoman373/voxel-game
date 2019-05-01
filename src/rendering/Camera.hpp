#pragma once

#include "Math.hpp"

class Camera {
public:
    vec3 position = vec3(0.0f, 0.0f, 0.0f);
    vec3 direction = vec3(0.0f, 0.0f, -1.0f);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);

    float nearPlane = 0.1f;
    float farPlane = 10000.0f;

    float fov = 70.0f;

    /** If true, no perspective will be used, and the output will be "2D" **/
    bool orthographic = false;

    /** Represents the screen resolution for use in calculating the aspect ratio for projection **/
    vec2 dimensions;

public:
    Camera();

    Camera(vec3 pos);

    const vec3 &getPosition() const;

    const vec3 &getDirection() const;

    /**
     * Returns the vector that is perpendicular to the look vector and the up vector (a.k.a "right").
     */
    vec3 getRight() const;

    vec3 getUp() const;

    void setPosition(const vec3 &pos);

    void setDirection(const vec3 &dir);

    mat4 getView() const;

    mat4 getProjection() const;

    void updateSpaceCamera(float speed);

    void updateFpsCamera(float speed);

    void matchWindowResolution();
};
