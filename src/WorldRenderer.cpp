#include "WorldRenderer.hpp"
#include "World.hpp"

#include <crucible/Renderer.hpp>
#include <crucible/IBL.hpp>

static const std::vector<vec3> normalLookup = {
        {1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, -1.0f}
};

static const std::vector<vec3> tangentLookup = {
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, -1.0f},
        {-1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f}
};

WorldRenderer::WorldRenderer(World &world): world(world) {

}

WorldRenderer::~WorldRenderer() {
    running = false;

    if (thread0->joinable()) {
        thread0->join();
    }
    delete thread0;

    if (thread1->joinable()) {
        thread1->join();
    }
    delete thread1;

    if (thread2->joinable()) {
        thread2->join();
    }
    delete thread2;

    if (thread3->joinable()) {
        thread3->join();
    }
    delete thread3;
}

void WorldRenderer::init() {
    blockShader.loadFile("resources/blockShader.vsh", "resources/blockShader.fsh");
    skyboxShader.loadFile("resources/skybox.vsh", "resources/skybox.fsh");

    texture.load("resources/terrain.png", true);
    texture_r.load("resources/terrain_r.png", true);
    texture_m.load("resources/terrain_m.png", true);
    texture_e.load("resources/terrain_e.png", true);
    texture_n.load("resources/terrain_n.png", true);


    nearMaterial.setPBRUniforms(texture, texture_r, texture_m, texture_n);
    nearMaterial.setUniformTexture("emissionTex", texture_e, 5);
    nearMaterial.setShader(blockShader);

    for (int i = 0; i < normalLookup.size(); i++) {
        nearMaterial.setUniformVec3("normalLookup[" + std::to_string(i) + "]", normalLookup[i]);
    }
    for (int i = 0; i < tangentLookup.size(); i++) {
        nearMaterial.setUniformVec3("tangentLookup[" + std::to_string(i) + "]", tangentLookup[i]);
    }

    Renderer::setSkyboxShader(skyboxShader);
    IBL::generateIBLmaps(vec3(0.0f,  0.0f, 0.0f), Renderer::irradiance, Renderer::specular);

    thread0 = new std::thread([&]() {
        remeshThread(remeshQueue0);
    });
    thread0->detach();

    thread1 = new std::thread([&]() {
        remeshThread(remeshQueue1);
    });
    thread1->detach();

    thread2 = new std::thread([&]() {
        remeshThread(remeshQueue2);
    });
    thread2->detach();

    thread3 = new std::thread([&]() {
        remeshThread(remeshQueue3);
    });
    thread3->detach();
}

void WorldRenderer::render() {
    int regeneratedChunks = 0;

    for (auto &i : chunkRenderers) {
        ChunkRenderer *c = i.second;

        int x = i.first.x;
        int z = i.first.y;

        if (doesChunkHaveNeighbors(x, z)) {
            if (c->attachedChunk.isDirty) {
                c->attachedChunk.isDirty = false;

                int remainderX = x % 2;
                int remainderZ = z % 2;

                ChunkRemeshJob job = {
                        c,
                        world.getChunk(x+1, z),
                        world.getChunk(x-1, z),

                        world.getChunk(x, z+1),
                        world.getChunk(x, z-1),

                        world.getChunk(x+1, z+1),
                        world.getChunk(x+1, z-1),

                        world.getChunk(x-1, z+1),
                        world.getChunk(x-1, z-1)
                };

                if (remainderX == 0 && remainderZ == 0) {
                    remeshQueue0.enqueue(job);
                }
                if (remainderX != 0 && remainderZ != 0) {
                    remeshQueue1.enqueue(job);
                }

                if (remainderX != 0 && remainderZ == 0) {
                    remeshQueue2.enqueue(job);
                }
                if (remainderX == 0 && remainderZ != 0) {
                    remeshQueue3.enqueue(job);
                }







//                if (regeneratedChunks < 5) {
//                    c->generateMesh();
//
//
//                    c->render(&nearMaterial);
//
//                    regeneratedChunks++;
//                }
            }
            c->render(&nearMaterial);
        }
    }
}

bool WorldRenderer::chunkRendererExists(int x, int z) {
    bool exists = chunkRenderers.find({x, z}) != chunkRenderers.end();

    return exists;
}

bool WorldRenderer::doesChunkHaveNeighbors(int x, int z) {
    bool posX = world.chunkExists(x+1, z);
    bool negX = world.chunkExists(x-1, z);

    bool posZ = world.chunkExists(x, z+1);
    bool negZ = world.chunkExists(x, z-1);

    bool posXposZ = world.chunkExists(x+1, z+1);
    bool posXnegZ = world.chunkExists(x+1, z-1);

    bool negXposZ = world.chunkExists(x-1, z+1);
    bool negXnegZ = world.chunkExists(x-1, z-1);

    if (posX && negX && posZ && negZ && posXposZ && posXnegZ && negXposZ && negXnegZ) {
        return true;
    }
    else {
        return false;
    }
}

ChunkRenderer* WorldRenderer::getChunkRenderer(int x, int z) {
    if (!chunkRendererExists(x, z)) {
        ChunkRenderer *cr = new ChunkRenderer(*world.getChunk(x, z));

        chunkRenderers.emplace(vec2i(x, z), cr);
    }

    return chunkRenderers[{x, z}];
}

void WorldRenderer::remeshThread(ReaderWriterQueue<ChunkRemeshJob> &queue) {
    while (running) {
        ChunkRemeshJob j;
        bool succeeded = queue.try_dequeue(j);

        if (succeeded) {
            //std::cout << "building mesh at: " << j.chunk->attachedChunk.chunk_x << ", " << j.chunk->attachedChunk.chunk_z << std::endl;

            j.chunk->generateMesh(j);
        }
    }
}