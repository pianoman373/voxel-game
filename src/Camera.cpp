#include "Camera.hpp"
#include "Input.hpp"

#include <iostream>

#include <GLFW/glfw3.h>

glm::vec3 Camera::getPosition() {
    return this->position;
}

glm::vec3 Camera::getDirection() {
    return this->direction;
}

void Camera::setPosition(const glm::vec3 &position) {
    this->position = position;
}

void Camera::setDirection(const glm::vec3 &direction) {
    this->direction = direction;
}

glm::mat4 Camera::getView() {
    return glm::lookAt(this->position, this->position + glm::normalize(direction), this->up);
}

glm::mat4 Camera::getProjection() {
    return glm::perspective(70.0f, 800.0f / 600.0f, 0.1f, 10000.0f);
}

glm::vec2 lastpos = glm::vec2();
bool first = true;

void Camera::update() {
    glm::vec3 right = glm::cross(this->up, this->direction);

    if (Input::isKeyDown(GLFW_KEY_A))
        this->position += right * MOVEMENT_SPEED;

    if (Input::isKeyDown(GLFW_KEY_D))
        this->position -= right * MOVEMENT_SPEED;

    if (Input::isKeyDown(GLFW_KEY_W))
        this->position += this->direction * MOVEMENT_SPEED;

    if (Input::isKeyDown(GLFW_KEY_S))
        this->position -= this->direction * MOVEMENT_SPEED;

    if (Input::isKeyDown(GLFW_KEY_R))
        this->position += this->up * MOVEMENT_SPEED;

    if (Input::isKeyDown(GLFW_KEY_F))
        this->position -= this->up * MOVEMENT_SPEED;

    glm::vec2 offset = Input::getCursorPos() - lastpos;

    float xOffset = -offset.x / 10;
    float yOffset = offset.y / 10;

    if (Input::isKeyDown(GLFW_KEY_UP)) {
        yOffset = -ROTATION_SPEED;
    }
    if (Input::isKeyDown(GLFW_KEY_DOWN)) {
        yOffset = ROTATION_SPEED;
    }
    if (Input::isKeyDown(GLFW_KEY_RIGHT)) {
        xOffset = -ROTATION_SPEED;
    }
    if (Input::isKeyDown(GLFW_KEY_LEFT)) {
        xOffset = ROTATION_SPEED;
    }

    //vertical
    glm::mat4 mat = glm::mat4();
    mat = glm::rotate(mat, glm::radians(yOffset), right);

    glm::vec4 vec = mat * glm::vec4(this->direction, 1.0f);

    this->direction = glm::normalize(glm::vec3(vec));

    glm::vec4 vec2 = mat * glm::vec4(this->up, 1.0f);

    //this->up = glm::normalize(glm::vec3(vec2));

    //horizontal
    glm::mat4 mat2 = glm::mat4();
    mat2 = glm::rotate(mat2, glm::radians(xOffset), this->up);
    glm::vec4 vec3 = mat2 * glm::vec4(this->direction, 1.0f);
    this->direction = glm::normalize(glm::vec3(vec3));

    lastpos = Input::getCursorPos();
}
