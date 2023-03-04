#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <cstddef>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "program.h"
#include "uniform.h"
#include "heightmap.h"
class nc2VertexType {
    public:
    glm::vec3 pos;
    glm::vec3 hsv;
    static void registerType()
    {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(nc2VertexType),
                              (const void *)offsetof(nc2VertexType, pos));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(nc2VertexType),
                              (const void *)offsetof(nc2VertexType, hsv));
        glEnableVertexAttribArray(1);
    }
};
class NewChunk2 {
    const static int sideWidth = 256;
    const static int maxBlockCount = 0xffffff;

    int maxVertexCount = 0;
    int vertexCount = 0;
    int blockCount = 0;

    int *blocks = nullptr;
    nc2VertexType *vertices = nullptr;

    int heightmapXOffset = 0;
    int heightmapYOffset = 0;
    int heightmapZOffset = 0;
    

    uint32_t vao = 0;
    uint32_t vbo = 0;
    Program &p;

   public:
    NewChunk2(Program &p_p);
    int *getBlock(int x, int y, int z);

    ~NewChunk2();
    void prepare();
    int countBlocks();
    void setProgram(Program &p_p);
    void render();
    void loadFromHeightmap(Heightmap &heightmap, int xoffset = 0, int zoffset = 0);
    int  *newBlock(int x, int y, int z);

   private:
    void prepareMesh();
    nc2VertexType &newVertex(const glm::vec3 &x);
    void newQuad(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c,
                 const glm::vec3 &d);
    void darkenVertices();
    void snow();
};
