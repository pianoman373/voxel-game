#include "util/Input.hpp"
#include <imgui.h>

#include <GLFW/glfw3.h>

#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

static bool keyDown[1024] = {false};
static bool keyPressed[1024] = {false};
static bool mouseDown[4] = {false};
static bool mousePressed[4] = {false};

static double xpos = 0;
static double ypos = 0;

static double last_xpos = 0;
static double last_ypos = 0;

static GLFWwindow* window;
static bool cursor = true;
static float scroll = 0;

static std::string charBuffer;


static std::function<void(int)> keyPressedCallback;
static std::function<void(unsigned int)> charPressedCallback;

namespace Input {
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
        if (action != GLFW_REPEAT) {
            if (key >= 0 && key < 1024)
            {
                if (action == GLFW_PRESS) {
                    keyDown[key] = true;
                    keyPressed[key] = true;
                }
                    
                else if (action == GLFW_RELEASE) {
                    keyDown[key] = false;
                }
            }
        }

        if (cursor)
            ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mode);

        if (keyPressedCallback && (action == GLFW_PRESS || action == GLFW_REPEAT))
            keyPressedCallback(key);
	}

    void char_callback(GLFWwindow *window, unsigned int codepoint) {
        charBuffer.push_back((char)codepoint);

        if (cursor)
            ImGui_ImplGlfw_CharCallback(window, codepoint);

        if (charPressedCallback)
            charPressedCallback(codepoint);
    }

    void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
        if (action != GLFW_REPEAT) {
            if (button >= 0 && button < 3)
            {
                if (action == GLFW_PRESS) {
                    mouseDown[button] = true;
                    mousePressed[button] = true;
                }
                else if (action == GLFW_RELEASE)
                    mouseDown[button] = false;
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
        return keyDown[key];
    }

    bool isKeyPressed(int key) {
        return keyPressed[key];
    }

    std::string getCharPresses() {
        return charBuffer;
    }

    bool isMouseButtonDown(int button) {
        return mouseDown[button];
    }

    bool isMouseButtonPressed(int button) {
        return mousePressed[button];
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

    void setMouseGrabbed(bool grabbed) {
        cursor = !grabbed;
        glfwSetInputMode(window, GLFW_CURSOR, cursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
}

void Input::update() {
    memset(keyPressed, 0, sizeof(keyPressed));
    memset(mousePressed, 0, sizeof(mousePressed));
    charBuffer.clear();

    scroll = 0;
}