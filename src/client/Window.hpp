#pragma once

#include "Math.hpp"
#include <string>

struct GLFWwindow;

class Window {
private:


public:
    static GLFWwindow *window;


    static void create(const vec2i &resolution, const std::string &title, bool fullscreen, bool vsync=true);

    static bool isOpen();

    static void begin();

    static void end();

    static void terminate();

    /**
     * Returns the time in seconds that the window has been open.
     */
    static double getTime();

    static float deltaTime();

    /**
     * Returns the window size in pixels.
     */
    static vec2i getWindowSize();

    static float getAspectRatio();

    static void setMouseGrabbed(bool grabbed);

    static bool getMouseGrabbed();
};
