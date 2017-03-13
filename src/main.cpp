#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include <GLFW/glfw3.h>
#include <iostream>

#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

#include "Input.hpp"
#include "Game.hpp"

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1500, 900, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
	    std::cout << "Failed to create GLFW window" << std::endl;
	    glfwTerminate();
	    return -1;
	}
	glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

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

    Game game(window);

    game.init();

	while(!glfwWindowShouldClose(window))
	{
        game.update();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplGlfwGL3_NewFrame();

        game.render();
        ImGui::Render();

	    glfwSwapBuffers(window);
        glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
