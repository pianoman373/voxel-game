#include "Camera.hpp"
#include "Window.hpp"

vec3 Camera::getPosition() {
    return this->position;
}

vec3 Camera::getDirection() {
    return this->direction;
}

vec3 Camera::getRight() {
    return normalize(cross(this->direction, this->up));
}

vec3 Camera::getUp() {
    return normalize(cross(getRight(), this->direction));
}

void Camera::setPosition(const vec3 &position) {
    this->position = position;
}

void Camera::setDirection(const vec3 &direction) {
    this->direction = direction;
}

mat4 Camera::getView() {
    return LookAt(this->position, this->position + normalize(direction), getUp());
}

mat4 Camera::getProjection() {
    vec2i size = Window::getWindowSize();
    return perspective(70, (float)size.x / (float)size.y, 0.1f, 10000.0f);
}
