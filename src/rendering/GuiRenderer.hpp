#pragma once

#include "Math.hpp"
#include "rendering/Texture.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/Font.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Resources.hpp"

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