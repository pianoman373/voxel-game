#pragma once

#include <crucible/Mesh.hpp>
#include <crucible/Shader.hpp>

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

    void renderBlockItem(int blockID, float x, float y, float size);
};