#pragma once

#include "Chunk.hpp"
#include <mutex>

struct RegionCache {
    FILE *pFile;
};

class ChunkIO {
private:
    std::map<std::string, FILE*> saveBuffer;

    int rleEncode(uint8_t *input, int inputLength, uint8_t *output);

    int rleDecode(uint8_t *input, int inputLength, uint8_t *output);

    void encodeInt(uint16_t val, unsigned char *buffer);

    uint16_t decodeInt(unsigned char *buffer);

    FILE *getFile(std::string filename);

    void resizeSector(FILE *pFile, int chunkIndex, int growAmount);

    std::mutex mx;

public:
    void saveChunk(Chunk *c);

    void loadChunk(Chunk *c);

    bool chunkHasFile(Chunk *c);

    bool chunkIsSaved(Chunk *c);

    void flushSaveBuffer();
};