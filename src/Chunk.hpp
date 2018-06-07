#pragma once

#define CHUNK_SIZE 32

#include <crucible/Mesh.hpp>
#include <crucible/MeshFactory.hpp>
#include <crucible/Material.hpp>

#include "ChunkMesh.hpp"

#include <mutex>

class ChunkManager;


class Chunk {
private:
    ChunkManager *cm;

    int getBlockFromWorld(int x, int y, int z);
	bool rebuild = false;


public:
	unsigned char blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE] = { 0 };
	unsigned char lightMap[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = { 0 };

    ChunkMesh mesh;
    bool empty = true;
    bool isDirty = false;
    bool generated = false;

    //TODO: should be a vec3i
    int chunk_x;
    int chunk_y;
    int chunk_z;

	int getSunlight(int x, int y, int z);

	void setSunlight(int x, int y, int z, int val);
	
	int getTorchlight(int x, int y, int z);
	
	void setTorchlight(int x, int y, int z, int val);
	
	void placeTorch(int x, int y, int z);

	void removeTorch(int x, int y, int z);

    Chunk(int x, int y, int z, ChunkManager* cm);

    ~Chunk();

    char getBlock(int x, int y, int z);

    void setBlock(int x, int y, int z, char block);

    /**
     * Regenerates what should actually be rendered. Required for block
     * changes to appear.
     */
    void generateMesh();

	void render(Material *mat);
};
