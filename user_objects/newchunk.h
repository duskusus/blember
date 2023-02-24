#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <cstddef>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "ibo.h"
#include "program.h"
#include "uniform.h"
#include "vertexbuffer.h"

class block {
   public:
    // position of block and type
    glm::vec3 position;
    glm::vec3 type;

   private:
    // shader layout locations
    const static int blockPos = 0;
    const static int blockType = 1;

   public:
    static void registerType()
    {
        glVertexAttribPointer(blockPos, 3, GL_FLOAT, GL_FALSE, sizeof(block),
                              (const void *)offsetof(block, position));
        glEnableVertexAttribArray(blockPos);

        glVertexAttribPointer(blockType, 3, GL_FLOAT, GL_FALSE,
                              sizeof(block),
                              (const void *)offsetof(block, type));
        glEnableVertexAttribArray(blockType);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(blockType, 1);
        glVertexAttribDivisor(blockPos, 1);
    }
};

class NewChunk {
    block *blocks;

    uint32_t vertexBuffer = 0;
    uint32_t instanceBuffer = 0;
    uint32_t indexBuffer = 0;
    uint32_t vao = 0;
    const Program &sp;

    const int vertPos = 2;
    const int blockType = 1;
    const int blockPos = 0;
    Uniform &u_model;

   public:
    uint32_t renderableBlockCount = 0;
    uint32_t chunksize;
    uint32_t stacklimit = 100;  // max number of vertically adjacent blocks that
                                // will be used to fill holes
    const uint32_t blockcount = chunksize * chunksize * stacklimit;
    const uint32_t colcount = chunksize * chunksize;
    int *heightmap = nullptr;
        int averageBlockHeight = 0;


    NewChunk(Program &p_sp, Uniform &p_model, uint32_t p_chunksize);

    block nullblock;
    block &at(int index);
    void generate();
void convolveHeightmap(uint32_t kwid);
    void fastNoise();
    void trees(int count);
    void normalizeHeightmap();
    int *getHeightmapPtr(int x, int y);
     int getHeightmapVal(int x, int y);
     int *getHeightmapPtr(int x, int y, int *h);
    void slowNoise(const float sloperange, int count, const int sizeOffset, const int sizeRange);
    int verifyNonOverlap();
    void clear();
    block &newBlock(const glm::vec3 &position);
    block &newBlock(const glm::vec3 &position, const glm::vec3 &type);
    block &noverlapNewBlock(const glm::vec3 &position, const glm::vec3 &type);
    void render();
    void sync();
    ~NewChunk();
};