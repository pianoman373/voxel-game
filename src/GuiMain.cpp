#include "GuiMain.hpp"

#include "core/GuiRenderer.hpp"
#include "core/Window.hpp"
#include "core/Input.hpp"

bool GuiMain::button(vec2 position, vec2 size) {
    vec4 color = vec4(0.5f, 0.5f, 0.5f, 1.0f);
    vec2 cursorPos = Input::getCursorPos();
    vec2 halfSize = size / 2.0f;

    if (cursorPos.x > position.x - halfSize.x && cursorPos.x < position.x + halfSize.x) {
        if (cursorPos.y > position.y - halfSize.y && cursorPos.y < position.y + halfSize.y) {
            color = vec4(0.7f, 0.7f, 0.7f, 1.0f);
        }
    }


    GuiRenderer::renderSprite(position, size, color);




    return false;
}

void GuiMain::init() {
    font.loadFromFile("resources/arial.ttf", 16.0f);
}

void GuiMain::render() {
    vec2i resolution = Window::getWindowSize();

    GuiRenderer::renderSprite(vec2(resolution.x / 2.0f, resolution.y / 2.0f), vec2(resolution.x, resolution.y), vec4(0.1f, 0.1f, 0.1f, 1.0f));

    button(vec2(100.0f, 500.0f), vec2(200.0f, 40.0f));

    GuiRenderer::renderText(vec2(100.0f, 500.0f), "New Game", font, vec4(1.0f));
}