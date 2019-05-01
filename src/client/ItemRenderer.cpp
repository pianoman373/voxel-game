#include "client/ItemRenderer.hpp"
#include "common/Block.hpp"
#include "client/Client.hpp"

#include "rendering/Primitives.hpp"
#include "client/Window.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Resources.hpp"

ItemRenderer::ItemRenderer(Client &client): client(client) {

}

void ItemRenderer::init() {
    mesh = Primitives::cube();

    itemShader = Resources::getShader("resources/item.vsh", "resources/item.fsh");
}

void ItemRenderer::renderBlockItem(Block &block, float x, float y, float size) {
    vec2 position = vec2(x, y);

    itemShader.bind();
    vec2i res = Window::getWindowSize();

    mat4 model;

    model = translate(model, vec3(position.x, position.y, 0.0f));
    model = scale(model, vec3(size, size, 0.0f));
    model = rotate(model, vec3(1.0f, 0.0f, 0.0f), 30.0f);
    model = rotate(model, vec3(0.0f, 1.0f, 0.0f), 45.0f);


    mat4 view;

    mat4 projection = orthographic(0.0f, res.x, 0.0f, res.y, -1.0f, 1.0f);

    vec2i textureCoord = block.getTextureCoord(EnumDirection::POSITIVE_Y);

    float textureSize = 16.0f;

    vec2 uv1 = vec2((textureCoord.x / textureSize), (textureCoord.y / textureSize));
    vec2 uv2 = vec2(((textureCoord.x + 1) / textureSize), ((textureCoord.y + 1) / textureSize));

    itemShader.uniformMat4("model", model);
    itemShader.uniformMat4("view", view);
    itemShader.uniformMat4("projection", projection);
    itemShader.uniformVec4("uvOffsets", vec4(uv1.x, uv1.y, uv2.x, uv2.y));

    client.worldRenderer.texture.bind();

    mesh.render();
}