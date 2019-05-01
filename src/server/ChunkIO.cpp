#include "server/ChunkIO.hpp"

#include <iostream>
#include <fstream>

#include <stdio.h>

inline bool fileExists(const std::string& name) {
    std::ifstream infile(name);
    return infile.good();
}

void ChunkIO::encodeInt(uint16_t val, unsigned char *buffer)
{
    buffer[0] = val & 0xff;
    buffer[1] = (val>>8)  & 0xff;
}

uint16_t ChunkIO::decodeInt(unsigned char *buffer) {
    return ((((unsigned int)buffer[0])) | (((unsigned int)buffer[1]) << 8));
}

#define LOOKUP_TABLE_SIZE 768
#define SECTOR_SIZE 1024

FILE *ChunkIO::getFile(std::string filename) {
    bool exists;
    FILE *pFile;

    if (saveBuffer.find(filename) != saveBuffer.end()) {
        pFile = saveBuffer[filename];
        exists = true;
    }
    else {
        exists = fileExists(filename);

        if (exists) {
            pFile = fopen (filename.c_str(), "rb+");
            //std::cout << "opening file: " << filename << std::endl;
        } else {
            pFile = fopen (filename.c_str(), "wb+");
            //std::cout << "creating file: " << filename << std::endl;

            //write the lookup table for the first time
            char *memblock = new char[LOOKUP_TABLE_SIZE];

            for (int i = 0; i < LOOKUP_TABLE_SIZE; i++) {
                memblock[i] = 0x00;
            }

            //myfile.write(memblock, size);
            fwrite(memblock, 1, LOOKUP_TABLE_SIZE, pFile);

            delete[] memblock;
        }

        saveBuffer[filename] =  pFile;
    }

    return pFile;
}

void ChunkIO::resizeSector(FILE *pFile, int chunkIndex, int growAmount) {
    //std::cout << "resizing sector by " << growAmount << " sectors" << std::endl;

    //read the lookup table to see if this chunk is in there
    unsigned char buffer[2] = {0};
    fseek(pFile, chunkIndex, SEEK_SET);
    fread(buffer, 1, 2, pFile);
    unsigned char lookupSectors;
    fread(&lookupSectors, 1, 1, pFile);

    int index = decodeInt(buffer);

    int startIndex = LOOKUP_TABLE_SIZE + ((index-1) * SECTOR_SIZE) + (lookupSectors * SECTOR_SIZE);

    fseek(pFile, 0, SEEK_END);

    int endIndex = ftell(pFile);

    int amountToRead = endIndex - startIndex;


    unsigned char *buffer2 = new unsigned char[amountToRead];

    fseek(pFile, startIndex, SEEK_SET);
    fread(buffer2, 1, amountToRead, pFile);

    fseek(pFile, startIndex + (growAmount * SECTOR_SIZE), SEEK_SET);
    fwrite(buffer2, 1, amountToRead, pFile);


    //update the lookup table because chunks have shifted


    //read the entire table into a local buffer
    unsigned char *lookupTable = new unsigned char[LOOKUP_TABLE_SIZE];
    fseek(pFile, 0, SEEK_SET);
    fread(lookupTable, 1, LOOKUP_TABLE_SIZE, pFile);

    unsigned char decodeBuffer[2] = {0};

    //update the sector size of this chunk
    //std::cout << "updating chunk size from " << (int)lookupTable[chunkIndex + 2] << " to " << lookupTable[chunkIndex + 2] + growAmount << std::endl;
    lookupTable[chunkIndex + 2] = lookupTable[chunkIndex + 2] + growAmount;

    int thisChunkSector = decodeInt(&lookupTable[chunkIndex]);

    //std::cout << "this chunk's sector is " << thisChunkSector << std::endl;

    //update all indices in the buffer that are ahead of this updated chunk
    for (int i = 0; i < LOOKUP_TABLE_SIZE; i += 3) {
        int index = decodeInt(&lookupTable[i]);

        if (index != 0) {
            //std::cout << "checking index " << index << std::endl;

            if (index > thisChunkSector) {
                //std::cout << "updating index from " << index << " to " << index + growAmount << std::endl;

                encodeInt(index + growAmount, decodeBuffer);

                lookupTable[i] = decodeBuffer[0];
                lookupTable[i+1] = decodeBuffer[1];
            }
        }
    }

    //now write the edited buffer back to the file
    fseek(pFile, 0, SEEK_SET);
    fwrite(lookupTable, 1, LOOKUP_TABLE_SIZE, pFile);



    delete[] buffer2;
    delete[] lookupTable;
    //std::cout << "completed sector resizing" << std::endl;
}

void ChunkIO::saveChunk(std::shared_ptr<Chunk> c) {
    mx.lock();
    std::string filename = "save/" + std::to_string(c->chunk_x >> 4) + "_" + std::to_string(c->chunk_z >> 4) + ".voxeldata";

    //std::cout << "----------------------------------" << std::endl;

    FILE * pFile = getFile(filename);


    int chunkIndex = ((c->chunk_x & 15) + (c->chunk_z & 15) * 16) * 3;

    //std::cout << "chunk pos (" << c->chunk_x << ", " << c->chunk_y << ", " << c->chunk_z << ")" << std::endl;
    //std::cout << "region pos (" << (c->chunk_x & 15) << ", " << (c->chunk_y & 15) << ", " << (c->chunk_z & 15) << ")" << std::endl;
    //std::cout << "chunk index: " << chunkIndex << std::endl;

    //read the lookup table to see if this chunk is in there
    unsigned char buffer[2] = {0};
    fseek(pFile, chunkIndex, SEEK_SET);
    fread(buffer, 1, 2, pFile);
    unsigned char lookupSectors;
    fread(&lookupSectors, 1, 1, pFile);

    int index = decodeInt(buffer);
    //std::cout << "offset from lookup table is " << index << std::endl;
    //std::cout << "original sector size is " << (int)lookupSectors << std::endl;

    if (index != 0) {
        fseek(pFile, LOOKUP_TABLE_SIZE + ((index-1) * SECTOR_SIZE), SEEK_SET);
    }
    else {
        fseek(pFile, 0, SEEK_END);
    }

    int pos = ftell(pFile);
    int currentSector = ((pos - LOOKUP_TABLE_SIZE)/SECTOR_SIZE)+1;

    //std::cout << pos << std::endl;
    //std::cout << currentSector << std::endl;

    uint8_t compressedChunk[16*256*16*2];
    int length = c->serialize(compressedChunk);

    int sectors = (length / SECTOR_SIZE) + 1;

    //std::cout << "saving " << length << " bytes of chunk data, which is " << sectors << " sectors" << std::endl;
    //std::cout << "total chunk size is " << sectors * SECTOR_SIZE << " and padding is " << (sectors * SECTOR_SIZE) - length << " bytes" << std::endl;

    if (index != 0 && sectors > lookupSectors) {
        resizeSector(pFile, chunkIndex, sectors-lookupSectors);

        fseek(pFile, LOOKUP_TABLE_SIZE + ((index-1) * SECTOR_SIZE), SEEK_SET);
    }

    //write the chunk
    fwrite((char *)compressedChunk, 1, length, pFile);

    //write padding
    for (int i = 0; i < (sectors * SECTOR_SIZE) - length; i++) {
        fputc(0, pFile);
    }

    if (index == 0) {
        encodeInt(currentSector, buffer);

        //std::cout << "offset: " << chunkIndex << std::endl;

        fseek(pFile, chunkIndex, SEEK_SET);
        fputc(buffer[0], pFile);
        fputc(buffer[1], pFile);
        fputc(sectors, pFile);
    }

    //std::cout << "----------------------------------" << std::endl;

    c->changedFromDisk = false;

    mx.unlock();
}

static uint8_t compressedChunk[32*SECTOR_SIZE];

void ChunkIO::loadChunk(std::shared_ptr<Chunk> c) {
    mx.lock();
    if (!c) {
        mx.unlock();
        return;
    }

    //std::cout << "----------------------------------" << std::endl;

    std::string filename = "save/" + std::to_string(c->chunk_x >> 4) + "_" + std::to_string(c->chunk_z >> 4) + ".voxeldata";

    //std::cout << "loading chunk " << filename << std::endl;

    FILE * pFile = getFile(filename);

    int chunkIndex = ((c->chunk_x & 15) + (c->chunk_z & 15) * 16) * 3;
    //std::cout << "chunkIndex: " << chunkIndex << std::endl;

    unsigned char buffer[2];
    unsigned char sectors;
    fseek(pFile, chunkIndex, SEEK_SET);
    int amountRead = fread(buffer, 1, 2, pFile);


    fread(&sectors, 1, 1, pFile);

    if (amountRead == 0) {
        std::cout << "error: " << feof(pFile) << std::endl;
        std::cout << "error: " << ferror(pFile) << std::endl;

        exit(0);
    }

    //std::cout << "amount read: " << amountRead << std::endl;
    //std::cout << "buffer: " << (int)buffer[0] << ", " << (int)buffer[1] << std::endl;

    int index = decodeInt(buffer);

    //std::cout << "index: " << index << std::endl;
    //std::cout << "sectors: " << (int)sectors << std::endl;

    if (index != 0) {
        fseek(pFile, LOOKUP_TABLE_SIZE + ((index-1) * SECTOR_SIZE), SEEK_SET);

        //std::cout << LOOKUP_TABLE_SIZE + ((index-1) * SECTOR_SIZE) << std::endl;

        

        fread(compressedChunk, 1, sectors*SECTOR_SIZE, pFile);

        c->unSerialize(compressedChunk, sectors*SECTOR_SIZE);
    }

    //std::cout << "----------------------------------" << std::endl;
    mx.unlock();
}

bool ChunkIO::chunkHasFile(std::shared_ptr<Chunk> c) {
    std::string filename = "save/" + std::to_string(c->chunk_x >> 5) + "_" + std::to_string(c->chunk_z >> 5) + ".voxeldata";

    return fileExists(filename);
}

bool ChunkIO::chunkIsSaved(std::shared_ptr<Chunk> c) {
    std::string filename = "save/" + std::to_string(c->chunk_x >> 5) + "_" + std::to_string(c->chunk_z >> 5) + ".voxeldata";

    if (fileExists(filename)) {
        FILE * pFile = getFile(filename);

        int chunkIndex = ((c->chunk_x & 15) + (c->chunk_z & 15) * 16) * 3;

        unsigned char buffer[2] = {0};
        unsigned char sectors = 0;
        fseek(pFile, chunkIndex, SEEK_SET);
        fread(buffer, 1, 2, pFile);
        fread(&sectors, 1, 1, pFile);

        int index = decodeInt(buffer);


        return index != 0;
    }

    return false;
}

void ChunkIO::flushSaveBuffer() {
    //std::cout << "flushing save buffer" << std::endl;

    for ( auto it = saveBuffer.begin(); it != saveBuffer.end(); it++ )
    {
        fclose(it->second);
        //std::cout << "closing " << it->first << std::endl;
    }
    saveBuffer.clear();
}