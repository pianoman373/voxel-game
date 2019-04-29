#pragma once

#include "Math.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"
#include "Font.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include "Resources.hpp"

struct Sprite {
    vec2 position;
    vec2 size;
    vec4 uvs;
    vec4 color;
    const Texture &tex;
};

class GuiRenderer {
private:


public:
    static void renderSprite(vec2 position, vec2 size, vec4 uvs, vec4 color, const Texture &tex, Shader &shader=Resources::spriteShader);

    static void renderSprite(vec2 position, vec2 size, vec4 color, Shader &shader=Resources::spriteShader);

    static void renderText(vec2 position, const std::string &text, const Font &font, vec4 color);
};