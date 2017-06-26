#pragma once

#include "Math.hpp"
#include <string>

struct GLFWwindow;

class Window {
public:
    GLFWwindow *window;

public:
    Window();

    void create(vec2i resolution, std::string title);

    bool isOpen();

    void begin();

    /**
     * Updates input events.
     */
    void pollEvents();

    void update();

    void terminate();

    /**
     * Returns the time in seconds that the window has been open.
     */
    float getTime();
};
