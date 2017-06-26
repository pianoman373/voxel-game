#include "Camera.hpp"
#include "Input.hpp"

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
    //TODO: get rid of magic resolution numbers
    return perspective(70.0f, 1400.0f / 800.0f, 0.1f, 10000.0f);
}

vec2 lastpos;
bool first = true;

//TODO This is just for debug purposes, should be removed.
void Camera::update(GLFWwindow *window, float delta) {
    glfwSetInputMode(window, GLFW_CURSOR, true ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

    if (true) {
        vec3 right = cross(this->up, this->direction);
        //float speed = 8.0f * delta;

//        if (Input::isKeyDown(GLFW_KEY_A))
//            this->position = this->position + right * speed;

//        if (Input::isKeyDown(GLFW_KEY_D))
//            this->position = this->position - right * speed;

//        if (Input::isKeyDown(GLFW_KEY_W))
//            this->position = this->position + this->direction * speed;

//        if (Input::isKeyDown(GLFW_KEY_S))
//            this->position = this->position - this->direction * speed;

//        if (Input::isKeyDown(GLFW_KEY_R))
//            this->position = this->position + this->up * speed;

//        if (Input::isKeyDown(GLFW_KEY_F))
//            this->position = this->position - this->up * speed;

        //rotation
        vec2 offset = Input::getCursorPos() - lastpos;
        float xOffset = -offset.x / 10;
        float yOffset = offset.y / 10;

        //vertical
        mat4 mat;
        mat = rotate(mat, right, yOffset);

        vec4 vec = mat * vec4(this->direction, 1.0f);

        this->direction = normalize(vec3(vec));

        //horizontal
        mat4 mat2 = mat4();
        mat2 = rotate(mat2, this->up, xOffset);
        vec4 vecj = mat2 * vec4(this->direction, 1.0f);
        this->direction = normalize(vec3(vecj));
    }

    lastpos = Input::getCursorPos();
}

void Camera::updateMouse() {

}
