#pragma once


#include <crucible/Shader.hpp>
#include <crucible/Texture.hpp>
#include <crucible/Material.hpp>
#include <crucible/Camera.hpp>
#include <crucible/DirectionalLight.hpp>

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

    Shader blockShader;
    Shader skyboxShader;



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

    DirectionalLight sun{normalize(vec3(-0.4f, -0.6f, -1.0f)), vec3(1.4f, 1.3f, 1.0f) * 3.0f, 2048};

public:
    Texture texture;
    Texture texture_r;
    Texture texture_m;
    Texture texture_e;
    Texture texture_n;

    WorldRenderer(World &world);

    ~WorldRenderer();

    void init();

    void render(Camera &cam);

    bool chunkRendererExists(int x, int z);

    void deleteChunkRenderer(int x, int z);

    bool doesChunkHaveNeighbors(int x, int z);

    std::shared_ptr<ChunkRenderer> getChunkRenderer(int x, int z);

    void remeshThread(BlockingReaderWriterQueue<ChunkRemeshJob> &queue);
};