#pragma once

#include <memory>
#include "common/Block.hpp"

class Chunk;

class ChunkNeighborhood {
private:
    std::shared_ptr<Chunk> center;

    std::shared_ptr<Chunk> posX;
    std::shared_ptr<Chunk> negX;

    std::shared_ptr<Chunk> posZ;
    std::shared_ptr<Chunk> negZ;

    std::shared_ptr<Chunk> posXposZ;
    std::shared_ptr<Chunk> posXnegZ;

    std::shared_ptr<Chunk> negXposZ;
    std::shared_ptr<Chunk> negXnegZ;

    void lookupCallback(int x, int y, int z, std::function<void(std::shared_ptr<Chunk>, int, int, int)> callback);
public:
    
    ChunkNeighborhood(std::shared_ptr<Chunk> center, 
                      std::shared_ptr<Chunk> posX, 
                      std::shared_ptr<Chunk> negX,
                      std::shared_ptr<Chunk> posZ,
                      std::shared_ptr<Chunk> negZ,
                      std::shared_ptr<Chunk> posXposZ,
                      std::shared_ptr<Chunk> posXnegZ,
                      std::shared_ptr<Chunk> negXposZ,
                      std::shared_ptr<Chunk> negXnegZ);

    ChunkNeighborhood();

    Block &getBlock(int x, int y, int z);

    int getBlockID(int x, int y, int z);

    int getHeight(int x, int z);

    int getSunlight(int x, int y, int z);

    void setSunlight(int x, int y, int z, int val);

    int getTorchlight(int x, int y, int z);

    void setTorchlight(int x, int y, int z, int val);
};