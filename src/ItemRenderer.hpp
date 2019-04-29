#pragma once

#include "core/Mesh.hpp"
#include "core/Shader.hpp"

class Block;
class Client;

class ItemRenderer {
private:
    Mesh mesh;
    Shader itemShader;
    Client &client;

public:
    ItemRenderer(Client &client);

    void init();

    void renderBlockItem(Block &block, float x, float y, float size);
};