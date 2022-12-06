#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <cstddef>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "cube.h"
#include "ibo.h"
#include "program.h"
#include "uniform.h"
#include "vertexbuffer.h"

#define chunksize 16
#define blockcount chunksize *chunksize *chunksize

class block {
    // position within chunk
   public:
    glm::vec3 position = glm::vec3(0.0, 0.0, 0.0);
    uint32_t type = 0;

   private:
    const static int blockLocation = 0;
    const static int blockType = 1;

   public:
    static void registerType(const Program &sp)
    {
        glVertexAttribPointer(blockLocation, 4, GL_FLOAT, GL_FALSE,
                              sizeof(block),
                              (const void *)offsetof(block, position));

        glVertexAttribPointer(blockType, 1, GL_UNSIGNED_INT, GL_FALSE,
                              sizeof(block),
                              (const void *)offsetof(block, type));
    }

    static void enable()
    {
        glEnableVertexAttribArray(blockLocation);
        glEnableVertexAttribArray(blockType);
    }
};

class NewChunk {
    block *blocks;

    uint32_t blockCount = 0;
    uint32_t vertexBuffer = 0;
    uint32_t instanceBuffer = 0;
    uint32_t indexBuffer = 0;
    uint32_t vao = 0;
    const Program &sp;

    const int vertPos = 3;

   public:
    NewChunk(const Program &p_sp) : sp(p_sp)
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glm::mat4 modelView(1.0);
        blocks = new block[chunksize];

        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                     GL_STATIC_DRAW);
        glVertexAttribPointer(vertPos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                              0);
        glEnableVertexAttribArray(vertPos);

        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                     GL_STATIC_DRAW);

        glGenBuffers(1, &instanceBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
        glBufferData(GL_ARRAY_BUFFER, blockcount * sizeof(block), blocks,
                     GL_STATIC_DRAW);
        glVertexAttribDivisor(instanceBuffer, 8);

        block::enable();

        block::registerType(sp);  // make shader program aware of existence
        // of block type
    }

    block &at(int index)
    {
        if (index > 0 && index < chunksize) {
            return blocks[index];
        }
    }

    block &newBlock(const glm::vec3 &position)
    {
        block &b = blocks[blockCount];
        b.position = position;
        blockCount ++;
        return b;
    }

    void render()
    {

        glBindVertexArray(vao);
        sp.use();
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);


        glDrawElementsInstanced(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT,
                                0, blockCount);
    }

    void sync()
    {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, chunksize * sizeof(block), blocks,
                     GL_STATIC_DRAW);
    }
};