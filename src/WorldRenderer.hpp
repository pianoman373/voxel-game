#pragma once


#include <crucible/Shader.hpp>
#include <crucible/Texture.hpp>
#include <crucible/Material.hpp>

#include <unordered_map>
#include <thread>

#include "ChunkRenderer.hpp"

#include "readerwriterqueue.h"

class World;

struct ChunkRemeshJob {
    std::shared_ptr<ChunkRenderer> renderer;

    std::shared_ptr<Chunk> center;

    std::shared_ptr<Chunk> posX;
    std::shared_ptr<Chunk> negX;

    std::shared_ptr<Chunk> posZ;
    std::shared_ptr<Chunk> negZ;

    std::shared_ptr<Chunk> posXposZ;
    std::shared_ptr<Chunk> posXnegZ;

    std::shared_ptr<Chunk> negXposZ;
    std::shared_ptr<Chunk> negXnegZ;
};

using namespace moodycamel;

class WorldRenderer {
private:
    World &world;

    Shader blockShader;
    Shader skyboxShader;

    Texture texture;
    Texture texture_r;
    Texture texture_m;
    Texture texture_e;
    Texture texture_n;

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

public:
    WorldRenderer(World &world);

    ~WorldRenderer();

    void init();

    void render();

    bool chunkRendererExists(int x, int z);

    bool doesChunkHaveNeighbors(int x, int z);

    std::shared_ptr<ChunkRenderer> getChunkRenderer(int x, int z);

    void remeshThread(BlockingReaderWriterQueue<ChunkRemeshJob> &queue);
};