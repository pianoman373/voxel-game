#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const float MOVEMENT_SPEED = 0.1f;
const float ROTATION_SPEED = 0.01f;

class Camera {
private:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

public:
    glm::vec3 getPosition();

    glm::vec3 getDirection();

    void setPosition(const glm::vec3 &position);

    void setDirection(const glm::vec3 &direction);

    glm::mat4 getView();

    glm::mat4 getProjection();

    void update();
};
