#pragma once

#include "Math.hpp"

class GLFWwindow;

namespace Input {

    //private callback functions
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

    void char_callback(GLFWwindow* window, unsigned int codepoint);

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    void cursor_callback(GLFWwindow* window, double xpos, double ypos);

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    bool isKeyDown(int key);

    bool isMouseButtonDown(int button);

    float getScroll();

    vec2 getCursorPos();

    vec2 getLastCursorPos();

    void setWindowInstance(GLFWwindow* windowInstance);

    float getTime();

    bool isMouseGrabbed();
}
