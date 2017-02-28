#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include <GLFW/glfw3.h>
#include <iostream>

#include "Input.hpp"
#include "Game.hpp"

#include "AntTweakBar.h"

Game game;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
	    std::cout << "Failed to create GLFW window" << std::endl;
	    glfwTerminate();
	    return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, Input::key_callback);
    glfwSetCursorPosCallback(window, Input::cursor_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, 0, 0);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Initialize the GUI
    TwInit(TW_OPENGL_CORE, NULL);
    TwWindowSize(800, 600);
    TwBar * EulerGUI = TwNewBar("Euler settings");
    TwBar * QuaternionGUI = TwNewBar("Quaternion settings");
    TwSetParam(EulerGUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
    TwSetParam(QuaternionGUI, NULL, "position", TW_PARAM_CSTRING, 1, "808 16");

    game.init();

	while(!glfwWindowShouldClose(window))
	{
	    glfwPollEvents();

        game.update();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        game.render();

        TwDraw();

	    glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
