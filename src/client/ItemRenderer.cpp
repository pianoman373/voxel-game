#include "client/ItemRenderer.hpp"
#include "common/Block.hpp"
#include "common/World.hpp"
#include "client/Client.hpp"

#include "rendering/Primitives.hpp"
#include "client/Window.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Resources.hpp"

ItemRenderer::ItemRenderer(Client &client): client(client) {

}

void ItemRenderer::init() {
    mesh = Primitives::cube();

    itemShader.loadFile("mods/base/resources/shaders/item.glsl");
}

void ItemRenderer::renderBlockItem(Block &block, float x, float y, float size) {
    vec2 position = vec2(x, y);

    itemShader.bind();
    vec2i res = Window::getWindowSize();

    mat4 model;

    model = model * translate(vec3(position.x, position.y, 0.0f));
    model = model * scale(vec3(size, size, 0.0f));
    model = model * rotate(vec3(1.0f, 0.0f, 0.0f), 30.0f);
    model = model * rotate(vec3(0.0f, 1.0f, 0.0f), 45.0f);

    mat4 view;

    mat4 projection = orthographic(0.0f, res.x, 0.0f, res.y, -1.0f, 1.0f);

    int textureIndex = block.getTextureIndex(EnumDirection::POSITIVE_Y);

    itemShader.uniformMat4("model", model);
    itemShader.uniformMat4("view", view);
    itemShader.uniformMat4("projection", projection);
    itemShader.uniformFloat("textureIndex", textureIndex);

    client.worldRenderer.atlas.getTexture().bind();

    mesh.render();
}