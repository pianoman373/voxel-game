#pragma once


#include "core/Shader.hpp"
#include "core/Texture.hpp"
#include "core/Material.hpp"
#include "core/Camera.hpp"
#include "core/DirectionalLight.hpp"

#include <unordered_map>
#include <thread>

#include "ChunkRenderer.hpp"
#include "World.hpp"
#include "readerwriterqueue.h"

class World;

struct ChunkRemeshJob {
    std::shared_ptr<ChunkRenderer> renderer;

    ChunkNeighborhood neighborhood;
};

using namespace moodycamel;

class WorldRenderer {
private:
    World &world;
    Client &client;

    Shader blockShader;
    Material skyboxMaterial;



    Material nearMaterial;

    std::unordered_map<vec2i, std::shared_ptr<ChunkRenderer>> chunkRenderers;

    BlockingReaderWriterQueue<ChunkRemeshJob> remeshQueue0;
    BlockingReaderWriterQueue<ChunkRemeshJob> remeshQueue1;
    BlockingReaderWriterQueue<ChunkRemeshJob> remeshQueue2;
    BlockingReaderWriterQueue<ChunkRemeshJob> remeshQueue3;

    bool running = true;
    std::thread *thread0;
    std::thread *thread1;
    std::thread *thread2;
    std::thread *thread3;

    DirectionalLight *sun = nullptr;

public:
    Texture texture;
    Texture texture_r;
    Texture texture_m;
    Texture texture_e;
    Texture texture_n;

    WorldRenderer(World &world, Client &client);

    ~WorldRenderer();

    void init();

    void render(Camera &cam);

    bool chunkRendererExists(int x, int z);

    void deleteChunkRenderer(int x, int z);

    bool doesChunkHaveNeighbors(int x, int z);

    std::shared_ptr<ChunkRenderer> getChunkRenderer(int x, int z);

    void remeshThread(BlockingReaderWriterQueue<ChunkRemeshJob> &queue);
};