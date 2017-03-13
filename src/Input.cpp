#include "Input.hpp"
#include <iostream>

bool keys[1024];

double xpos = 0;
double ypos = 0;

double last_xpos = 0;
double last_ypos = 0;

namespace Input {
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
        if (action != GLFW_REPEAT) {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

            if (key >= 0 && key < 1024)
            {
                if (action == GLFW_PRESS)
                    keys[key] = true;
                else if (action == GLFW_RELEASE)
                    keys[key] = false;
            }
        }
	}

    void char_callback(GLFWwindow *window, unsigned int codepoint) {

    }

    void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {

    }

    void cursor_callback(GLFWwindow* window, double x, double y) {
        last_xpos = xpos;
        last_ypos = ypos;

        xpos = x;
        ypos = y;
    }

    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {

    }

    bool isKeyDown(int key) {
        return keys[key];
    }

    glm::vec2 getCursorPos() {
        return glm::vec2(xpos, ypos);
    }

    glm::vec2 getLastCursorPos() {
        return glm::vec2(last_xpos, last_ypos);
    }
}
