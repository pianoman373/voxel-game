#include "Window.hpp"
#include "GLFW/glfw3.h"
#include "Input.hpp"

#include <glad/glad.h>
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

GLFWwindow *Window::window;

void Window::create(vec2i resolution, std::string title) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSwapInterval(1);

    window = glfwCreateWindow(resolution.x, resolution.y, title.c_str(), nullptr, nullptr);

    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
	gladLoadGL();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, true);

    glfwSetKeyCallback(window, Input::key_callback);
    glfwSetMouseButtonCallback(window, Input::mouse_button_callback);
    glfwSetCursorPosCallback(window, Input::cursor_callback);
    glfwSetCharCallback(window, Input::char_callback);
    glfwSetScrollCallback(window, Input::scroll_callback);

    glfwSetCursorPos(window, 0, 0);

    Input::setWindowInstance(window);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool Window::isOpen() {
    return !glfwWindowShouldClose(window);
}

void Window::begin() {
    glClearColor(0.0f ,0.0f , 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplGlfwGL3_NewFrame();

    Input::update();
    glfwPollEvents();
}

void Window::end() {
    ImGui::Render();

    glfwSwapBuffers(window);
}

void Window::terminate() {
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}

float Window::getTime() {
    return glfwGetTime();
}

vec2i Window::getWindowSize() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return vec2i(width, height);
}
