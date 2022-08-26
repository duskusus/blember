#pragma once
#include <string>
#include "program.h"
class VertexBuffer {
    std::string name;
    GLuint program;
    GLint location;
    GLuint bufferID;
    GLuint vaoID;
    //same order as parameters to VertexBufferPointer
    unsigned int size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    void *data = nullptr;


    unsigned int count;

    public:
    VertexBuffer(const std::string &n, Program &p);
    void set(void *data, unsigned int count);
    void buffer();
    void enable();
    void bind();
    void upload();
    void vertexSpec(unsigned int p_size, GLenum type, GLboolean normalized, GLsizei stride);
    void supplyData();
    void makeInstanced();
    ~VertexBuffer();
};