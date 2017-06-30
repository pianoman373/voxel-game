#include "Camera.hpp"
#include "Input.hpp"
#include "Client.hpp"

#include <iostream>

#include <GLFW/glfw3.h>

vec3 Camera::getPosition() {
    return this->position;
}

vec3 Camera::getDirection() {
    return this->direction;
}

vec3 Camera::getRight() {
    return cross(this->direction, this->up);
}

void Camera::setPosition(const vec3 &position) {
    this->position = position;
}

void Camera::setDirection(const vec3 &direction) {
    this->direction = direction;
}

mat4 Camera::getView() {
    return LookAt(this->position, this->position + normalize(direction), this->up);
}

mat4 Camera::getProjection() {
    vec2i size = Client::window.getWindowSize();
    std::cout << size.x << ", " << size.y << std::endl;
    return perspective(70.0f, (float)size.x / (float)size.y, 0.1f, 10000.0f);
}
