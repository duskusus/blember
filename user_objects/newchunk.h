#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <cstddef>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "cube.h"
#include "ibo.h"
#include "program.h"
#include "uniform.h"
#include "vertexbuffer.h"

class block {
   public:
    // position of block and type
    glm::vec3 position;
    uint32_t type = 0;

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

        glVertexAttribPointer(blockType, 1, GL_UNSIGNED_INT, GL_FALSE,
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
    uint32_t stacklimit = 10; // how many blocks can we have stacked on top of each other
    uint32_t blockcount = chunksize * chunksize * stacklimit;
    int *heightmap = nullptr;

    NewChunk(Program &p_sp, Uniform &p_model, uint32_t p_chunksize)
        : sp(p_sp), u_model(p_model), chunksize(p_chunksize)
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glm::mat4 modelView(1.0);
        blocks = new block[blockcount];

        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                     GL_STATIC_DRAW);
        glEnableVertexAttribArray(vertPos);
        glVertexAttribPointer(vertPos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                              0);

        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                     GL_STATIC_DRAW);

        glGenBuffers(1, &instanceBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);

        block::registerType();  // make shader program aware of existence
        // of block type

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    }

    block nullblock;
    block &at(int index)
    {
        if (index > 0 && index < blockcount) {
            return blocks[index];
        }
        return nullblock;
    }
    void generate()
    {
        heightmap = new int[blockcount];

        lowFreqNoise();
        convolveHeightmap();
        normalizeHeightmap();
        clear();

        for (int x = 0; x < chunksize; x++) {
            for (int z = 0; z < chunksize; z++) {
                int neighbors[4];

                int p = getHeightmapVal(x, z);

                neighbors[0] = getHeightmapVal(x + 1, z);
                neighbors[1] = getHeightmapVal(x - 1, z);
                neighbors[2] = getHeightmapVal(x, z + 1);
                neighbors[3] = getHeightmapVal(x, z - 1);

                newBlock(glm::vec3(x, p, z));

                int min = p;

                for(int i = 0; i < 4; i++) {
                    if(min > neighbors[i]) min = neighbors[i];
                }

                if(p - min > stacklimit) continue;
                for(int h = min; h < p; h++) {
                    newBlock(glm::vec3(x, h, z));
                }

            }
        }
    }
    void convolveHeightmap()
    {
        uint32_t kwid = 25;

        int *new_heightmap = new int[blockcount];

        for (int x = 0; x < chunksize; x++) {
            for (int y = 0; y < chunksize; y++) {
                int total = 0;
                uint32_t blocks_sampled = 0;

                for (int i = 0; i < kwid; i++) {
                    for (int j = 0; j < kwid; j++) {
                        int *h =
                            getHeightmapPtr(x + i - kwid / 2, y + j - kwid / 2);
                        if (h != NULL) {
                            total += *h;
                            blocks_sampled++;
                        }
                    }
                }

                if (blocks_sampled > 0) {
                    const int newval = total / blocks_sampled;
                    *getHeightmapPtr(x, y, new_heightmap) = newval;
                }
                else {
                    *getHeightmapPtr(x, y, new_heightmap) =
                        *getHeightmapPtr(x, y);
                }
            }
        }
        delete[] heightmap;
        heightmap = new_heightmap;
    }

    void normalizeHeightmap()
    {
        int min = 0;
        int sum = 0;
        for (int i = 0; i < blockcount; i++) {
            if (heightmap[i] < min) min = heightmap[i];
            sum += heightmap[i];
        }
        int average = sum / blockcount;
        if(min < 0){
        for (int i = 0; i < blockcount; i++) {
            heightmap[i] -= min;
        }
        }
    }

    inline int *getHeightmapPtr(int x, int y)
    {
        return getHeightmapPtr(x, y, heightmap);
    }

    inline int getHeightmapVal(int x, int y)
    {
        int *h = getHeightmapPtr(x, y);
        if (h) return *h;
        return 0;
    }

    inline int *getHeightmapPtr(int x, int y, int *h)
    {
        if (x < 0 || x >= chunksize || y < 0 || y >= chunksize) return NULL;
        const int index = x + y * chunksize;
        return &h[index];
    }

    void lowFreqNoise()
    {
        for (int i = 0; i < 50; i++) {
            const int disturbMagnitude = 20;
            const float sloperange = 0.02;

            int x = rand() % chunksize;
            int y = rand() % chunksize;

            int disturbSize = rand() % chunksize;
            int disturbHeight = rand() % disturbMagnitude;
            float disturbSlopeX =
                float(rand()) / float(RAND_MAX) * sloperange - sloperange / 2.0;
            float disturbSlopeY =
                float(rand()) / float(RAND_MAX) * sloperange - sloperange / 2.0;

            for (int q = 0; q < disturbSize; q++) {
                for (int p = 0; p < disturbSize; p++) {
                    int xx = x + q - disturbSize / 2;
                    int yy = y + p - disturbSize / 2;
                    int *h = getHeightmapPtr(xx, yy);
                    if (h)
                        *h += disturbHeight + xx * disturbSlopeX +
                              yy * disturbSlopeY;
                }
            }
        }
    }
    void clear() { renderableBlockCount = 0; }

    block &newBlock(const glm::vec3 position)
    {
        block &b = at(renderableBlockCount);
        b.position = position;
        b.type = position.y + rand() % 50;
        if(renderableBlockCount < blockcount - 1) renderableBlockCount++;
        return b;
    }

    void render()
    {
        glBindVertexArray(vao);
        sp.use();
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glDrawElementsInstanced(GL_TRIANGLES, 6 * 2 * 3, GL_UNSIGNED_INT, 0,
                                renderableBlockCount);
    }

    void sync()
    {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
        glBufferData(GL_ARRAY_BUFFER, renderableBlockCount * sizeof(block),
                     blocks, GL_STATIC_DRAW);
    }

    ~NewChunk()
    {
        if (heightmap != nullptr) delete[] heightmap;
        delete[] blocks;
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &indexBuffer);
        glDeleteBuffers(1, &instanceBuffer);
        glDeleteVertexArrays(1, &vao);
    }
};