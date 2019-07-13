#include "common/ChunkNeighborhood.hpp"
#include "common/World.hpp"

void ChunkNeighborhood::lookupCallback(int x, int y, int z, std::function<void(std::shared_ptr<Chunk>, int, int, int)> callback) {
    //center
    if (x >= 0 && x < 16 && z >= 0 && z < 16) {
        callback(center, x, y, z);
    }

    //positive X
    if (x >= 16 && z >= 0 && z < 16) {
        callback(posX, x - 16, y, z);
    }
    //negative X
    if (x < 0 && z >= 0 && z < 16) {
        callback(negX, x + 16, y, z);
    }

    //positive Z
    if (x >= 0 && x < 16 && z >= 16) {
        callback(posZ, x, y, z - 16);
    }
    //negative Z
    if (x >= 0 && x < 16 && z < 0) {
        callback(negZ, x, y, z + 16);
    }

    //positive X positive Z
    if (x >= 16 &&  z >= 16) {
        callback(posXposZ, x - 16, y, z - 16);
    }
    //positive X negative Z
    if (x >= 16 && z < 0) {
        callback(posXnegZ, x - 16, y, z + 16);
    }

    //negative X positive Z
    if (x < 0 &&  z >= 16) {
        callback(negXposZ, x + 16, y, z - 16);
    }
    //negative X negative Z
    if (x < 0 && z < 0) {
        callback(negXnegZ, x + 16, y, z + 16);
    }
}

ChunkNeighborhood::ChunkNeighborhood(std::shared_ptr<Chunk> center, 
                                     std::shared_ptr<Chunk> posX, 
                                     std::shared_ptr<Chunk> negX,
                                     std::shared_ptr<Chunk> posZ,
                                     std::shared_ptr<Chunk> negZ,
                                     std::shared_ptr<Chunk> posXposZ,
                                     std::shared_ptr<Chunk> posXnegZ,
                                     std::shared_ptr<Chunk> negXposZ,
                                     std::shared_ptr<Chunk> negXnegZ) {
    this->center = center;
    this->posX = posX;
    this->negX = negX;
    this->posZ = posZ;
    this->negZ = negZ;
    this->posXposZ = posXposZ;
    this->posXnegZ = posXnegZ;
    this->negXposZ = negXposZ;
    this->negXnegZ = negXnegZ;
}

ChunkNeighborhood::ChunkNeighborhood() {

}

Block &ChunkNeighborhood::getBlock(int x, int y, int z) {
    //center
    if (x >= 0 && x < 16 && z >= 0 && z < 16) {
        return center->getBlock(x, y, z);
    }

    //positive X
    if (x >= 16 && z >= 0 && z < 16) {
        return posX->getBlock(x - 16, y, z);
    }
    //negative X
    if (x < 0 && z >= 0 && z < 16) {
        return negX->getBlock(x + 16, y, z);
    }

    //positive Z
    if (x >= 0 && x < 16 && z >= 16) {
        return posZ->getBlock(x, y, z - 16);
    }
    //negative Z
    if (x >= 0 && x < 16 && z < 0) {
        return negZ->getBlock(x, y, z + 16);
    }

    //positive X positive Z
    if (x >= 16 &&  z >= 16) {
        return posXposZ->getBlock(x - 16, y, z - 16);
    }
    //positive X negative Z
    if (x >= 16 && z < 0) {
        return posXnegZ->getBlock(x - 16, y, z + 16);
    }

    //negative X positive Z
    if (x < 0 &&  z >= 16) {
        return negXposZ->getBlock(x + 16, y, z - 16);
    }
    //negative X negative Z
    if (x < 0 && z < 0) {
        return negXnegZ->getBlock(x + 16, y, z + 16);
    }


    return center->getBlock(x, y, z);
}

int ChunkNeighborhood::getHeight(int x, int z) {
    //center
    if (x >= 0 && x < 16 && z >= 0 && z < 16) {
        return center->getHeight(x, z);
    }

    //positive X
    if (x >= 16 && z >= 0 && z < 16) {
        return posX->getHeight(x - 16, z);
    }
    //negative X
    if (x < 0 && z >= 0 && z < 16) {
        return negX->getHeight(x + 16, z);
    }

    //positive Z
    if (x >= 0 && x < 16 && z >= 16) {
        return posZ->getHeight(x, z - 16);
    }
    //negative Z
    if (x >= 0 && x < 16 && z < 0) {
        return negZ->getHeight(x, z + 16);
    }

    //positive X positive Z
    if (x >= 16 &&  z >= 16) {
        return posXposZ->getHeight(x - 16, z - 16);
    }
    //positive X negative Z
    if (x >= 16 && z < 0) {
        return posXnegZ->getHeight(x - 16, z + 16);
    }

    //negative X positive Z
    if (x < 0 &&  z >= 16) {
        return negXposZ->getHeight(x + 16, z - 16);
    }
    //negative X negative Z
    if (x < 0 && z < 0) {
        return negXnegZ->getHeight(x + 16, z + 16);
    }


    return center->getHeight(x, z);
}

int ChunkNeighborhood::getBlockID(int x, int y, int z) {
    //center
    if (x >= 0 && x < 16 && z >= 0 && z < 16) {
        return center->getBlockID(x, y, z);
    }

    //positive X
    if (x >= 16 && z >= 0 && z < 16) {
        return posX->getBlockID(x - 16, y, z);
    }
    //negative X
    if (x < 0 && z >= 0 && z < 16) {
        return negX->getBlockID(x + 16, y, z);
    }

    //positive Z
    if (x >= 0 && x < 16 && z >= 16) {
        return posZ->getBlockID(x, y, z - 16);
    }
    //negative Z
    if (x >= 0 && x < 16 && z < 0) {
        return negZ->getBlockID(x, y, z + 16);
    }

    //positive X positive Z
    if (x >= 16 &&  z >= 16) {
        return posXposZ->getBlockID(x - 16, y, z - 16);
    }
    //positive X negative Z
    if (x >= 16 && z < 0) {
        return posXnegZ->getBlockID(x - 16, y, z + 16);
    }

    //negative X positive Z
    if (x < 0 &&  z >= 16) {
        return negXposZ->getBlockID(x + 16, y, z - 16);
    }
    //negative X negative Z
    if (x < 0 && z < 0) {
        return negXnegZ->getBlockID(x + 16, y, z + 16);
    }


    return center->getBlockID(x, y, z);
}

int ChunkNeighborhood::getSunlight(int x, int y, int z) {
    //center
    if (x >= 0 && x < 16 && z >= 0 && z < 16) {
        return center->getSunlight(x, y, z);
    }

    //positive X
    if (x >= 16 && z >= 0 && z < 16) {
        return posX->getSunlight(x - 16, y, z);
    }
    //negative X
    if (x < 0 && z >= 0 && z < 16) {
        return negX->getSunlight(x + 16, y, z);
    }

    //positive Z
    if (x >= 0 && x < 16 && z >= 16) {
        return posZ->getSunlight(x, y, z - 16);
    }
    //negative Z
    if (x >= 0 && x < 16 && z < 0) {
        return negZ->getSunlight(x, y, z + 16);
    }

    //positive X positive Z
    if (x >= 16 &&  z >= 16) {
        return posXposZ->getSunlight(x - 16, y, z - 16);
    }
    //positive X negative Z
    if (x >= 16 && z < 0) {
        return posXnegZ->getSunlight(x - 16, y, z + 16);
    }

    //negative X positive Z
    if (x < 0 &&  z >= 16) {
        return negXposZ->getSunlight(x + 16, y, z - 16);
    }
    //negative X negative Z
    if (x < 0 && z < 0) {
        return negXnegZ->getSunlight(x + 16, y, z + 16);
    }

    return 15;
}

void ChunkNeighborhood::setSunlight(int x, int y, int z, int val) {
    //center
    if (x >= 0 && x < 16 && z >= 0 && z < 16) {
        center->setSunlight(x, y, z, val);
    }

    //positive X
    if (x >= 16 && z >= 0 && z < 16) {
        posX->setSunlight(x - 16, y, z, val);
    }
    //negative X
    if (x < 0 && z >= 0 && z < 16) {
        negX->setSunlight(x + 16, y, z, val);
    }

    //positive Z
    if (x >= 0 && x < 16 && z >= 16) {
        posZ->setSunlight(x, y, z - 16, val);
    }
    //negative Z
    if (x >= 0 && x < 16 && z < 0) {
        negZ->setSunlight(x, y, z + 16, val);
    }

    //positive X positive Z
    if (x >= 16 &&  z >= 16) {
        posXposZ->setSunlight(x - 16, y, z - 16, val);
    }
    //positive X negative Z
    if (x >= 16 && z < 0) {
        posXnegZ->setSunlight(x - 16, y, z + 16, val);
    }

    //negative X positive Z
    if (x < 0 &&  z >= 16) {
        negXposZ->setSunlight(x + 16, y, z - 16, val);
    }
    //negative X negative Z
    if (x < 0 && z < 0) {
        negXnegZ->setSunlight(x + 16, y, z + 16, val);
    }
}

int ChunkNeighborhood::getTorchlight(int x, int y, int z) {
    //center
    if (x >= 0 && x < 16 && z >= 0 && z < 16) {
        return center->getTorchlight(x, y, z);
    }

    //positive X
    if (x >= 16 && z >= 0 && z < 16) {
        return posX->getTorchlight(x - 16, y, z);
    }
    //negative X
    if (x < 0 && z >= 0 && z < 16) {
        return negX->getTorchlight(x + 16, y, z);
    }

    //positive Z
    if (x >= 0 && x < 16 && z >= 16) {
        return posZ->getTorchlight(x, y, z - 16);
    }
    //negative Z
    if (x >= 0 && x < 16 && z < 0) {
        return negZ->getTorchlight(x, y, z + 16);
    }

    //positive X positive Z
    if (x >= 16 &&  z >= 16) {
        return posXposZ->getTorchlight(x - 16, y, z - 16);
    }
    //positive X negative Z
    if (x >= 16 && z < 0) {
        return posXnegZ->getTorchlight(x - 16, y, z + 16);
    }

    //negative X positive Z
    if (x < 0 &&  z >= 16) {
        return negXposZ->getTorchlight(x + 16, y, z - 16);
    }
    //negative X negative Z
    if (x < 0 && z < 0) {
        return negXnegZ->getTorchlight(x + 16, y, z + 16);
    }

    return 0;
}

void ChunkNeighborhood::setTorchlight(int x, int y, int z, int val) {
    //center
    if (x >= 0 && x < 16 && z >= 0 && z < 16) {
        center->setTorchlight(x, y, z, val);
    }

    //positive X
    if (x >= 16 && z >= 0 && z < 16) {
        posX->setTorchlight(x - 16, y, z, val);
    }
    //negative X
    if (x < 0 && z >= 0 && z < 16) {
        negX->setTorchlight(x + 16, y, z, val);
    }

    //positive Z
    if (x >= 0 && x < 16 && z >= 16) {
        posZ->setTorchlight(x, y, z - 16, val);
    }
    //negative Z
    if (x >= 0 && x < 16 && z < 0) {
        negZ->setTorchlight(x, y, z + 16, val);
    }

    //positive X positive Z
    if (x >= 16 &&  z >= 16) {
        posXposZ->setTorchlight(x - 16, y, z - 16, val);
    }
    //positive X negative Z
    if (x >= 16 && z < 0) {
        posXnegZ->setTorchlight(x - 16, y, z + 16, val);
    }

    //negative X positive Z
    if (x < 0 &&  z >= 16) {
        negXposZ->setTorchlight(x + 16, y, z - 16, val);
    }
    //negative X negative Z
    if (x < 0 && z < 0) {
        negXnegZ->setTorchlight(x + 16, y, z + 16, val);
    }
}