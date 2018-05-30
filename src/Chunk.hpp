#pragma once

#define CHUNK_SIZE 32

#include <crucible/Mesh.hpp>
#include <crucible/MeshFactory.hpp>
#include <crucible/Material.hpp>

class World;


class Chunk {
private:

    World *world;

    int getBlockFromWorld(int x, int y, int z);
	bool rebuild = false;


public:
	unsigned char blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE] = { 0 };
	unsigned char lightMap[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = { 0 };

    Mesh mesh;
    MeshFactory ms;
    bool empty = true;
	bool isMeshGenerated = false;
    bool isDirty = false;

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

    Chunk(int x, int y, int z, World* world);

    ~Chunk();

    char getBlock(int x, int y, int z);

    void setBlock(int x, int y, int z, char block);

    /**
     * Regenerates what should actually be rendered. Required for block
     * changes to appear.
     */
    void generateMesh();

	void applyMesh();

	void render(Material *mat);
};
