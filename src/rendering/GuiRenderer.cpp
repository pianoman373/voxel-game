#include "rendering/GuiRenderer.hpp"
#include "rendering/Primitives.hpp"
#include "rendering/Renderer.hpp"
#include "client/Window.hpp"

void GuiRenderer::renderSprite(vec2 position, vec2 size, vec4 uvs, vec4 color, const Texture &tex, Shader &shader) {
    shader.bind();
    vec2i res = Window::getWindowSize();

    mat4 model;

    model = translate(model, vec3(position.x, position.y, 0.0f));
    model = scale(model, vec3(size.x, size.y, 0.0f));

    mat4 view;

    mat4 projection = orthographic(0.0f, res.x, 0.0f, res.y, -1.0f, 1.0f);

    shader.uniformMat4("model", model);
    shader.uniformMat4("view", view);
    shader.uniformMat4("projection", projection);
    shader.uniformVec4("uvOffsets", uvs);
    shader.uniformVec4("color", color);
    shader.uniformFloat("textureStrength", 1.0f);
    tex.bind();

    Resources::spriteMesh.render();

}

void GuiRenderer::renderSprite(vec2 position, vec2 size, vec4 color, Shader &shader) {
    shader.bind();
    vec2i res = Window::getWindowSize();

    mat4 model;

    model = translate(model, vec3(position.x, position.y, 0.0f));
    model = scale(model, vec3(size.x, size.y, 0.0f));

    mat4 view;

    mat4 projection = orthographic(0.0f, res.x, 0.0f, res.y, -1.0f, 1.0f);

    shader.uniformMat4("model", model);
    shader.uniformMat4("view", view);
    shader.uniformMat4("projection", projection);
    shader.uniformVec4("color", color);
    shader.uniformFloat("textureStrength", 0.0f);
    Texture::bindNull();


    Resources::spriteMesh.render();

}

void GuiRenderer::renderText(vec2 position, const std::string &text, const Font &font, vec4 color) {
    const char *textRaw = text.c_str();

    float scale = 1.0f;
    float x = position.x;
    float y = position.y;

    while (*textRaw) {
        const Character &ch = font.characters.at(*textRaw);

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) + font.descender * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        renderSprite({xpos, ypos}, {w, h}, {0.0f, 0.0f, 1.0f, 1.0f}, color, ch.tex, Resources::textShader);

        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)

        ++textRaw;
    }
}