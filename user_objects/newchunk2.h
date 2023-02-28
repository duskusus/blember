#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <cstddef>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "program.h"
#include "uniform.h"
class NewChunk2 {
    const int sideWidth = 256;
    const int maxBlockCount = sideWidth * sideWidth * sideWidth;
    int maxVertexCount = 0;
    int vertexCount = 0;
    int blockCount = 0;

    int *blocks = nullptr;
    glm::vec3 *vertices = nullptr;

    int heightmapXOffset;
    int heightmapYOffset;
    int heightmapZOffset;
    int *heightmap;

    uint32_t vao = 0;
    uint32_t vbo = 0;
    Program &p;

   public:
    NewChunk2(int xOffset, int yOffset, int zOffset, int *p_heightmap,
              Program &p_p);
    int *getBlock(uint8_t x, uint8_t y, uint8_t z);

    ~NewChunk2();
    void prepare();
    int countBlocks();
    void setProgram(Program &p_p);
    void render();
    void loadFromHeightmap();
    int  *newBlock(int x, int y, int z);

   private:
    void prepareMesh();
    void newVertex(const glm::vec3 &x);
    void newQuad(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c,
                 const glm::vec3 &d);
};
