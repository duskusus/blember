#include <cstdint>
#include <glm/vec3.hpp>
#include <memory.h>
#include "vertexbuffer.hpp"
#include "ibo.hpp"
#include "program.hpp"
enum block {
  empty,
  solid,
};
class Chunk {
  const float vertices[3 * 8] = {1, 1, -1, 1,  -1, -1, 1, 1,  1,  1, -1,
                                 1, 1, 1,  -1, -1, 1,  1, -1, -1, 1};
  unsigned int indices[3 * 12] = {4, 2, 0, 2, 7, 3, 6, 5, 7, 1, 7, 5,
                                 0, 3, 1, 4, 1, 5, 4, 6, 2, 2, 6, 7,
                                 6, 4, 5, 1, 3, 7, 0, 2, 3, 4, 0, 1};
  unsigned int chunksize = 16 * 16 * 16;
  block *blocks = nullptr;
  Program &p;
  unsigned int vao = 0;
  VertexBuffer *vbo = nullptr;
  IBO *ibo = nullptr;

public:
  Chunk(Program &p_p): p(p_p) {
    blocks = new block[chunksize];
    for (int i = 0; i < chunksize; i++) {
      blocks[i] = empty;
    }
    vbo = new VertexBuffer("aPos", p);
    vbo->buffer();
    vbo->set((void*)vertices, 3 * 8);
    vbo->vertexSpec(3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
    vbo->enable();
    vbo->upload();
    ibo = new IBO(indices, 3 * 12);
  }
  void render() {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    vbo->bind();
    ibo->bind();
    glDrawElementsInstanced(GL_TRIANGLES, 3 * 12, GL_UNSIGNED_INT, 0, chunksize);
  }
  ~Chunk() { delete[] blocks; }
};