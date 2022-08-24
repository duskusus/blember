#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vertexbuffer.hpp"
#include "ibo.hpp"
#include "uniform.hpp"
#include "program.hpp"
class Chunk {
  const float vertices[3 * 8] = {1, 1, -1, 1, -1, -1, 1, 1, 1, 1, -1, 1, -1, 1, -1, -1, -1, -1, -1, 1, 1, -1, -1, 1};
  unsigned int indices[3 * 12] = {4, 2, 0, 2, 7, 3, 6, 5, 7, 1, 7, 5,
                                 0, 3, 1, 4, 1, 5, 4, 6, 2, 2, 6, 7,
                                 6, 4, 5, 1, 3, 7, 0, 2, 3, 4, 0, 1};

  unsigned int width = 16;
  unsigned int length = 16;
  unsigned int ChunkHeight = 16;
  unsigned int chunksize;
  glm::vec4 *blocks = nullptr;
  Program &p;
  unsigned int vao = 0;
  unsigned int vbo = 0;
  unsigned int ibo = 0;
  unsigned int instanceVbo = 0;
  unsigned int aPosId = 0;
  glm::mat4 modelMatrix = glm::mat4(1.0);
  Uniform &u_model;



public:
  Chunk(Program &p_p, Uniform &modelMatrix): p(p_p), u_model(modelMatrix) {
    chunksize = width * length * ChunkHeight;
    blocks = new glm::vec4[chunksize];
    for (int z = 0; z < length; z++) {
      for (int x = 0 ; x < width; x++) {
        unsigned int columnHeight = (rand() % ChunkHeight) + 1;
        for(int y = 0; y < columnHeight; y++) {
          setblock(x, y, z, glm::vec4(float(y) / 30.0 + 2.25, 0.0, 0.0, 1.0));
        }
      }
    }
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * 8 * sizeof(float), vertices, GL_STATIC_DRAW);
    aPosId = glGetAttribLocation(p.getID(), "aPos");
    glEnableVertexAttribArray(aPosId);
    glVertexAttribPointer(aPosId, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glGenBuffers(1, &instanceVbo);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glBufferData(GL_ARRAY_BUFFER, chunksize * 4 * sizeof(float), blocks, GL_STATIC_DRAW);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glVertexAttribDivisor(4, 1);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * 12 * sizeof(unsigned int), indices, GL_STATIC_DRAW);


  }
  void render() {
    u_model.set(glm::value_ptr(modelMatrix));
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDrawElementsInstanced(GL_TRIANGLES, 3 * 12, GL_UNSIGNED_INT, 0, chunksize);
  }
  void setblock(unsigned int x, unsigned int y, unsigned int z, glm::vec4 color) {
    const unsigned int blockIndex = x + y * 16 + z * 16 * 16;
    blocks[blockIndex] = color;
  }
  ~Chunk() { 
    delete[] blocks;
    glDeleteBuffers(1, &instanceVbo);
   }
   void move(unsigned int x, unsigned int y, unsigned int z) {
      x *= 16;
      y *= 16;
      z *= 16;
      modelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(x, y, z));
   }
};