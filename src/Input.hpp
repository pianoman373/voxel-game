#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>

namespace Input {
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

    void cursor_callback(GLFWwindow* window, double xpos, double ypos);

    bool isKeyDown(int key);

    glm::vec2 getCursorPos();

    glm::vec2 getLastCursorPos();
}
