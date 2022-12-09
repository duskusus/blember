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
    Program &sp;

    const int vertPos = 2;
    Uniform &u_model;

   public:
    NewChunk(Program &p_sp, Uniform &p_model) : sp(p_sp), u_model(p_model)
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glm::mat4 modelView(1.0);
        blocks = new block[chunksize];

        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                     GL_STATIC_DRAW);
                     
        glVertexAttribPointer(vertPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(vertPos);

        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                     GL_STATIC_DRAW);

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
        blockCount++;
        return b;
    }

    void render()
    {
        glBindVertexArray(vao);
        sp.use();
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        glDrawElementsInstanced(GL_LINES, 3 * 12, GL_UNSIGNED_INT, 0, 1);
    }

    void sync()
    {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, chunksize * sizeof(block), blocks,
                     GL_STATIC_DRAW);
    }
};