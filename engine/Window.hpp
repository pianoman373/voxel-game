#pragma once

#include "Math.hpp"
#include <string>

struct GLFWwindow;

class Window {
public:
    static GLFWwindow *window;

public:

    static void create(vec2i resolution, std::string title);

    static bool isOpen();

    static void begin();

    /**
     * Updates input events.
     */
    static void pollEvents();

    static void update();

    static void terminate();

    /**
     * Returns the time in seconds that the window has been open.
     */
    static float getTime();

    /**
     * Returns the window size in pixels.
     */
    static vec2i getWindowSize();
};
