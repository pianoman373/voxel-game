#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>

namespace Input {
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

    void char_callback(GLFWwindow* window, unsigned int codepoint);

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    void cursor_callback(GLFWwindow* window, double xpos, double ypos);

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    bool isKeyDown(int key);

    glm::vec2 getCursorPos();

    glm::vec2 getLastCursorPos();
}
