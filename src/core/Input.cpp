#include "Input.hpp"
#include <imgui.h>

#include <GLFW/glfw3.h>

#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

static bool keys[1024];
static bool mouse[4];

static double xpos = 0;
static double ypos = 0;

static double last_xpos = 0;
static double last_ypos = 0;

static GLFWwindow* window;
static bool cursor = true;
static float scroll = 0;


static std::function<void(int)> keyPressedCallback;
static std::function<void(unsigned int)> charPressedCallback;

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
            ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mode);

        if (keyPressedCallback && action == GLFW_PRESS)
            keyPressedCallback(key);
	}

    void char_callback(GLFWwindow *window, unsigned int codepoint) {
        if (cursor)
            ImGui_ImplGlfw_CharCallback(window, codepoint);

        if (charPressedCallback)
            charPressedCallback(codepoint);
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
            ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    }

    void cursor_callback(GLFWwindow* window, double x, double y) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        last_xpos = xpos;
        last_ypos = ypos;

        xpos = x;
        ypos = height - y;
    }

    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        scroll = yoffset;

        if (cursor)
            ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
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


    void registerKeyPressedCallback(std::function<void(int)> callback) {
	    keyPressedCallback = callback;
	}

    void registerCharPressedCallback(std::function<void(unsigned int)> callback) {
	    charPressedCallback = callback;
	}
}

void Input::update() {
    scroll = 0;
}