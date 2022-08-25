#include "ibo.h"
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
IBO::IBO(unsigned int *p_data, unsigned int p_size) {
  data = p_data;
  size = p_size;
  glGenBuffers(1, &id);
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(unsigned int), data, GL_STATIC_DRAW);
}
void IBO::bind() { 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    }
unsigned int IBO::getID() {
    return id;
}