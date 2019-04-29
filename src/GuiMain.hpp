#pragma once

#include "core/Font.hpp"

class GuiMain {
private:
    Font font;

    bool button(vec2 position, vec2 size);

public:
    void init();

    void render();
};