#include "client/WorldRenderer.hpp"
#include "common/World.hpp"
#include "client/Client.hpp"

#include "rendering/Renderer.hpp"
#include "rendering/IBL.hpp"
#include "rendering/Resources.hpp"


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

WorldRenderer::WorldRenderer(World &world, Client &client): world(world), client(client) {
    
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

    delete sun;
    delete ambient1;
    delete ambient2;
}

void WorldRenderer::init() {
    atlas.buildAtlas();
    
    if (client.settings.shadows) {
        sun = new DirectionalLight(normalize(vec3(-0.4f, -0.6f, -1.0f)), vec3(1.4f, 1.3f, 1.0f) * 3.5f, client.settings.shadow_resolution, 3, client.settings.render_distance*16.0f);
    }
    else {
        sun = new DirectionalLight(normalize(vec3(-0.4f, -1.6f, -0.6f)), vec3(1.4f, 1.3f, 1.0f));
    }
    
    blockShader.loadFile("mods/base/resources/shaders/blockShader.glsl");

    skyboxShader.loadFile("mods/base/resources/shaders/skybox.glsl");
    skyboxMaterial.deferred = false;
    skyboxMaterial.setShader(skyboxShader);
    skyboxMaterial.setUniformVec3("sun.direction", sun->m_direction);
    skyboxMaterial.setUniformVec3("sun.color", sun->m_color);

    nearMaterial.setUniformTextureArray("texArray", atlas.getTexture(), 0);
    nearMaterial.setShader(blockShader);

    for (int i = 0; i < normalLookup.size(); i++) {
        nearMaterial.setUniformVec3("normalLookup[" + std::to_string(i) + "]", normalLookup[i]);
    }
    for (int i = 0; i < tangentLookup.size(); i++) {
        nearMaterial.setUniformVec3("tangentLookup[" + std::to_string(i) + "]", tangentLookup[i]);
    }

    //liquidMaterial.setUniformTextureArray("texArray", atlas.getTexture(), 7);
    liquidShader.loadFile("mods/base/resources/shaders/liquidShader.glsl");
    liquidMaterial.setShader(liquidShader);

    for (int i = 0; i < normalLookup.size(); i++) {
        liquidMaterial.setUniformVec3("normalLookup[" + std::to_string(i) + "]", normalLookup[i]);
    }
    for (int i = 0; i < tangentLookup.size(); i++) {
        liquidMaterial.setUniformVec3("tangentLookup[" + std::to_string(i) + "]", tangentLookup[i]);
    }

    liquidMaterial.deferred = false;
    liquidMaterial.needsGbuffer = true;
    

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

    // Renderer::renderSkybox(&skyboxMaterial);
    // IBL::generateIBLmaps(vec3(0.0f,  0.0f, 0.0f), Renderer::irradiance, Renderer::specular);
    // Renderer::clear();
}

void WorldRenderer::render(Camera &cam) {
    int regeneratedChunks = 0;

    Renderer::renderDirectionalLight(sun);
    ///Renderer::renderDirectionalLight(ambient1);
    //Renderer::renderDirectionalLight(ambient2);

    for (auto &i : chunkRenderers) {
        std::shared_ptr<ChunkRenderer> cr = i.second;

        int x = i.first.x;
        int z = i.first.y;

        //Renderer::debug.renderDebugAABB(vec3(x * 16.0f, 0.0f, x * 16.0f), vec3((x+1) * 16.0f, 256.0f, (z+1) * 16.0f), vec3(0.0f, 1.0f, 0.0f));

        if (doesChunkHaveNeighbors(x, z)) {
            std::shared_ptr<Chunk> chunk = world.getChunk(x, z);

            if (chunk->isDirty) {
                chunk->isDirty = false;

                int remainderX = x % 2;
                int remainderZ = z % 2;

                ChunkRemeshJob job = {
                        cr,
                        world.getChunkNeighborhood(x, z)
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
            }
            cr->render(&nearMaterial, &liquidMaterial);
        }
    }
//
//    auto chunks = world.getChunks();
//
//    for (auto i : chunks) {
//        vec2i pos = i.first;
//
//        Renderer::debug.renderDebugAABB(vec3(pos.x * 16.0f, 0.0f, pos.y * 16.0f), vec3((pos.x+1) * 16.0f, 256.0f, (pos.y+1) * 16.0f), vec3(1.0f, 0.0f, 0.0f));
//    }

    Renderer::renderSkybox(&skyboxMaterial);
}

bool WorldRenderer::chunkRendererExists(int x, int z) {
    bool exists = chunkRenderers.find({x, z}) != chunkRenderers.end();

    return exists;
}

void WorldRenderer::deleteChunkRenderer(int x, int z) {
    if (chunkRendererExists(x, z)) {
        chunkRenderers.erase({x, z});
    }
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

std::shared_ptr<ChunkRenderer> WorldRenderer::getChunkRenderer(int x, int z) {
    if (!chunkRendererExists(x, z)) {
        ChunkRenderer *cr = new ChunkRenderer(x, z);

        chunkRenderers.emplace(vec2i(x, z), cr);
    }

    return chunkRenderers[{x, z}];
}

void WorldRenderer::remeshThread(BlockingReaderWriterQueue<ChunkRemeshJob> &queue) {
    while (running) {
        ChunkRemeshJob j;
        queue.wait_dequeue(j);

        j.renderer->generateMesh(j.neighborhood);
    }
}