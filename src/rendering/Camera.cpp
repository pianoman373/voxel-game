#include "rendering/Camera.hpp"
#include "client/Window.hpp"
#include "util/Input.hpp"

Camera::Camera() {

}

Camera::Camera(vec3 pos) {
    this->position = pos;
}

const vec3 &Camera::getPosition() const {
    return this->position;
}

const vec3 &Camera::getDirection() const {
    return this->direction;
}

vec3 Camera::getRight() const {
    return normalize(cross(this->direction, this->up));
}

vec3 Camera::getUp() const {
    return normalize(cross(getRight(), this->direction));
}

void Camera::setPosition(const vec3 &pos) {
    this->position = pos;
}

void Camera::setDirection(const vec3 &dir) {
    this->direction = dir;
}

mat4 Camera::getView() const {
    return LookAt(this->position, this->position + normalize(direction), getUp());
}

mat4 Camera::getProjection() const {
    vec2 size = dimensions;

    if (this->orthographic) {
        return ::orthographic(-size.x/2.0f, size.x/2.0f, -size.y/2.0f, size.y/2.0f, nearPlane, farPlane);
    }
    else {

        return perspective(fov, size.x / size.y, nearPlane, farPlane);
    }
}

void Camera::updateSpaceCamera(float speed) {
    static vec2 lastMousePos;
    Window::setMouseGrabbed(Input::isMouseButtonDown(1));

    if (Input::isMouseButtonDown(1)) {
        vec3 right = getRight();
        float deltaSpeed = speed * Window::deltaTime();

        if (Input::isKeyDown(Input::KEY_A))
            position = position - right * deltaSpeed;

        if (Input::isKeyDown(Input::KEY_D))
            position = position + right * deltaSpeed;

        if (Input::isKeyDown(Input::KEY_W))
            position = position + direction * deltaSpeed;

        if (Input::isKeyDown(Input::KEY_S))
            position = position - direction * deltaSpeed;

        if (Input::isKeyDown(Input::KEY_R))
            position = position + up * deltaSpeed;

        if (Input::isKeyDown(Input::KEY_F))
            position = position - up * deltaSpeed;


        //rotation
        vec2 offset = Input::getCursorPos() - lastMousePos;
        float xOffset = -offset.x / 10.0f;
        float yOffset = offset.y / 10.0f;
        float zOffset = 0.0f;

        if (Input::isKeyDown(Input::KEY_Q)) {
            zOffset -= 45.0f * Window::deltaTime();
        }
        if (Input::isKeyDown(Input::KEY_E)) {
            zOffset += 45.0f * Window::deltaTime();
        }

        //vertical
        mat4 mat;
        mat = rotate(mat, right, yOffset);
        vec4 vec = mat * vec4(direction, 1.0f);
        direction = normalize(vec3(vec));

        //horizontal
        mat = mat4();
        mat = rotate(mat, up, xOffset);
        vec4 vecj = mat * vec4(direction, 1.0f);
        direction = normalize(vec3(vecj));

        mat = mat4();
        mat = rotate(mat, direction, zOffset);
        vec4 veck = mat * vec4(right, 1.0f);
        right = normalize(vec3(veck));

        up = normalize(cross(right, direction));
    }
    lastMousePos = Input::getCursorPos();
}

void Camera::updateFpsCamera(float speed) {
    static vec2 lastMousePos;
    Window::setMouseGrabbed(Input::isMouseButtonDown(1));

    if (Input::isMouseButtonDown(1)) {
        vec3 right = getRight();
        float deltaSpeed = speed * Window::deltaTime();

        if (Input::isKeyDown(Input::KEY_A))
            position = position - right * deltaSpeed;

        if (Input::isKeyDown(Input::KEY_D))
            position = position + right * deltaSpeed;

        if (Input::isKeyDown(Input::KEY_W))
            position = position + direction * deltaSpeed;

        if (Input::isKeyDown(Input::KEY_S))
            position = position - direction * deltaSpeed;

        if (Input::isKeyDown(Input::KEY_R))
            position = position + up * deltaSpeed;

        if (Input::isKeyDown(Input::KEY_F))
            position = position - up * deltaSpeed;


        //rotation
        vec2 offset = Input::getCursorPos() - lastMousePos;
        float xOffset = -offset.x / 10.0f;
        float yOffset = offset.y / 10.0f;

        //vertical
        mat4 mat;
        mat = rotate(mat, right, yOffset);
        vec4 vec = mat * vec4(direction, 1.0f);
        direction = normalize(vec3(vec));

        //horizontal
        mat = mat4();
        mat = rotate(mat, up, xOffset);
        vec4 vecj = mat * vec4(direction, 1.0f);
        direction = normalize(vec3(vecj));

        up = vec3(0.0f, 1.0f, 0.0f);
    }
    lastMousePos = Input::getCursorPos();
}

void Camera::matchWindowResolution() {
    dimensions = {(float)Window::getWindowSize().x, (float)Window::getWindowSize().y};
}