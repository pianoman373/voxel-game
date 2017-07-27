#include "Input.hpp"
#include "../src/Client.hpp"
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

static bool keys[1024];
static bool mouse[4];

static double xpos = 0;
static double ypos = 0;

static double last_xpos = 0;
static double last_ypos = 0;

static GLFWwindow* window;
static bool cursor = true;
static float scroll = 0;

namespace Input {
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
        if (action != GLFW_REPEAT) {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                cursor = !cursor;
                glfwSetInputMode(window, GLFW_CURSOR, cursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
                //glfwSetWindowShouldClose(window, true);
            }

            if (key >= 0 && key < 1024)
            {
                if (action == GLFW_PRESS)
                    keys[key] = true;
                else if (action == GLFW_RELEASE)
                    keys[key] = false;
            }
        }

        if (cursor)
            ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mode);
	}

    void char_callback(GLFWwindow *window, unsigned int codepoint) {
        if (cursor)
            ImGui_ImplGlfwGL3_CharCallback(window, codepoint);
    }

    void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
        if (action != GLFW_REPEAT) {
            if (button >= 0 && button < 3)
            {
                if (action == GLFW_PRESS)
                    mouse[button] = true;
                else if (action == GLFW_RELEASE)
                    mouse[button] = false;
            }
        }

        if (cursor)
            ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
    }

    void cursor_callback(GLFWwindow* window, double x, double y) {
        last_xpos = xpos;
        last_ypos = ypos;

        xpos = x;
        ypos = y;
    }

    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        Client::scrollBlocks((int)yoffset);

        if (cursor)
            ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
    }

    bool isKeyDown(int key) {
        return keys[key];
    }

    bool isMouseButtonDown(int button) {
        return mouse[button];
    }

    float getScroll() {
        return scroll;
    }

    vec2 getCursorPos() {
        return vec2(xpos, ypos);
    }

    vec2 getLastCursorPos() {
        return vec2(last_xpos, last_ypos);
    }

    void setWindowInstance(GLFWwindow* windowInstance) {
        window = windowInstance;
    }

    float getTime() {
        return glfwGetTime();
    }

    bool isMouseGrabbed() {
        return !cursor;
    }
}
