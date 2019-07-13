#include "client/WorldRenderer.hpp"
#include "common/World.hpp"
#include "client/Client.hpp"

#include "rendering/Renderer.hpp"
#include "rendering/IBL.hpp"
#include "rendering/Resources.hpp"
#include "util/Input.hpp"
#include "optick.h"

#include "client/Window.hpp"


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
        sun = new DirectionalLight(normalize(vec3(-0.4f, -0.6f, -1.0f)), vec3(1.4f, 1.3f, 1.0f) * 4.0f, client.settings.shadow_resolution, 2, 16.0f*6);
    }
    else {
        sun = new DirectionalLight(normalize(vec3(-0.4f, -1.6f, -0.6f)), vec3(1.4f, 1.3f, 1.0f) * 3.0f);
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
        OPTICK_THREAD("Remesh 0");
        remeshThread(remeshQueue0);
    });
    thread0->detach();

    thread1 = new std::thread([&]() {
        OPTICK_THREAD("Remesh 1");
        remeshThread(remeshQueue1);
    });
    thread1->detach();

    thread2 = new std::thread([&]() {
        OPTICK_THREAD("Remesh 2");
        remeshThread(remeshQueue2);
    });
    thread2->detach();

    thread3 = new std::thread([&]() {
        OPTICK_THREAD("Remesh 3");
        remeshThread(remeshQueue3);
    });
    thread3->detach();

    Renderer::renderSkybox(&skyboxMaterial);
    IBL::generateIBLmaps(vec3(0.0f,  0.0f, 0.0f), Renderer::irradiance, Renderer::specular);
    Renderer::clear();
}

void WorldRenderer::render(Camera &cam) {
    quaternion q(vec3(1.0f, 0.f, 0.0f), Window::getTime()*0.1f);

    vec3 dir = q * vec3(0.0f, 0.0f, 1.0f);

    // sun->m_direction = dir;
    // skyboxMaterial.setUniformVec3("sun.direction", sun->m_direction);

    OPTICK_EVENT();
    int regeneratedChunks = 0;

    Renderer::renderDirectionalLight(sun);

    for (auto *i : world.entities) {
        Renderer::debug.renderDebugAABB(i->getPosition() - (vec3(i->width, i->height, i->depth) / 2.0f), i->getPosition() + (vec3(i->width, i->height, i->depth) / 2.0f), vec3(1.0f, 0.0f, 0.0f));
    }

    
    for (ChunkRenderer *i : rendererList) {
        //std::shared_ptr<ChunkRenderer> cr = i.second;
        int x = i->chunk_x;
        int z = i->chunk_z;

        // //Renderer::debug.renderDebugAABB(vec3(x * 16.0f, 0.0f, x * 16.0f), vec3((x+1) * 16.0f, 256.0f, (z+1) * 16.0f), vec3(0.0f, 1.0f, 0.0f));

        Chunk* chunk = i->chunk.get();

        if (chunk->hasNeighbors) {
            if (chunk->isDirty) {
                OPTICK_EVENT("isDirty");
                chunk->isDirty = false;

                int remainderX = x % 2;
                int remainderZ = z % 2;

                ChunkRemeshJob job = {
                        chunkRenderers[{x, z}],
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
            i->render(&nearMaterial, &liquidMaterial);
            
        }
    }

    Renderer::renderSkybox(&skyboxMaterial);
}

bool WorldRenderer::chunkRendererExists(int x, int z) {
    bool exists = chunkRenderers.find({x, z}) != chunkRenderers.end();

    return exists;
}

void WorldRenderer::deleteChunkRenderer(int x, int z) {
    if (chunkRendererExists(x, z)) {
        auto chunkRenderer = chunkRenderers[{x, z}];
        

        rendererList[chunkRenderer->rendererIndex] = rendererList.back();
        rendererList[chunkRenderer->rendererIndex]->rendererIndex = chunkRenderer->rendererIndex;
        rendererList.pop_back();

        
        deadRenderers.push_back(chunkRenderer);

        chunkRenderers.erase({x, z});
    }
}

std::shared_ptr<ChunkRenderer> WorldRenderer::getChunkRenderer(int x, int z) {
    if (!chunkRendererExists(x, z)) {

        std::shared_ptr<ChunkRenderer> cr;

        if (deadRenderers.empty()) {
            cr = std::shared_ptr<ChunkRenderer>(new ChunkRenderer(x, z, rendererList.size(), world.getChunk(x, z)));
        }
        else {
            cr = deadRenderers.back();
            deadRenderers.pop_back();

            cr->clear();

            cr->recycle(x, z, rendererList.size(), world.getChunk(x, z));
        }
        

        chunkRenderers.emplace(vec2i(x, z), cr);
        rendererList.push_back(cr.get());
    }

    return chunkRenderers[{x, z}];
}

void WorldRenderer::remeshThread(BlockingReaderWriterQueue<ChunkRemeshJob> &queue) {
    
    while (running) {
        ChunkRemeshJob j;
        queue.wait_dequeue(j);

        OPTICK_EVENT("Remesh");
        j.renderer->generateMesh(j.neighborhood);
    }
}