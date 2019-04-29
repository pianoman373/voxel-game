#include "Window.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Input.hpp"


#include <imgui.h>

#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

//Enable the dedicated GPU on Nvidia Optimus systems
#ifdef _WIN32
#include <windows.h>
extern "C" {
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

GLFWwindow *Window::window;

void Window::create(const vec2i &resolution, const std::string &title, bool fullscreen, bool vsync) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (fullscreen) {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        window = glfwCreateWindow(mode->width, mode->height, title.c_str(), monitor, nullptr);
    }
    else {
        window = glfwCreateWindow(resolution.x, resolution.y, title.c_str(), nullptr, nullptr);
    }


    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    glfwSwapInterval(vsync);
	gladLoadGL();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Setup ImGui binding
    ImGui::CreateContext();
#ifdef IMGUI_HAS_DOCK
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif


    ImGui::GetStyle().FrameRounding = 5.0f;
    ImGui::GetStyle().FramePadding.x = 8.0f;
    ImGui::GetStyle().FramePadding.y = 6.0f;

    ImVec4* colors = ImGui::GetStyle().Colors;

    colors[ImGuiCol_WindowBg]               = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_Border]                 = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.08f, 0.51f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.08f, 0.51f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.08f, 0.51f, 1.00f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.08f, 0.51f, 1.00f, 1.00f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.08f, 0.51f, 1.00f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.08f, 0.51f, 0.61f, 0.31f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);

    const char* glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);

    //ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    glfwSetKeyCallback(window, Input::key_callback);
    glfwSetMouseButtonCallback(window, Input::mouse_button_callback);
    glfwSetCursorPosCallback(window, Input::cursor_callback);
    glfwSetCharCallback(window, Input::char_callback);
    glfwSetScrollCallback(window, Input::scroll_callback);


    Input::setWindowInstance(window);
}

bool Window::isOpen() {
    return !glfwWindowShouldClose(window);
}

static float delta = 0;
static float lastFrameTime = 0;

void Window::begin() {
    float currentFrameTime = Window::getTime();
    delta = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;

    glClearColor(0.0f ,0.0f , 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Input::update();
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::end() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}

void Window::terminate() {
    ImGui_ImplOpenGL3_Shutdown();
    glfwTerminate();
}

float Window::getTime() {
    return glfwGetTime();
}

float Window::deltaTime() {
    return delta;
}

vec2i Window::getWindowSize() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return vec2i(width, height);
}

float Window::getAspectRatio() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return (float)width / (float)height;
}

void Window::setMouseGrabbed(bool grabbed) {
    glfwSetInputMode(window, GLFW_CURSOR, grabbed ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

bool Window::getMouseGrabbed() {
    return glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}