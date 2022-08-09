#include "vertexbuffer.hpp"
VertexBuffer::VertexBuffer(const std::string &n, Program &p) {
    name = n;
    program = p.getID();
    location = glGetAttribLocation(program, name.c_str());
    if(location == -1) {
        printf("%s is not a valid program variable!!\n", name.c_str());
    }
    glGenBuffers(1, &bufferID);

}
void VertexBuffer::set(void *data, unsigned int count) {
    bind();
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(type), data, GL_STATIC_DRAW);
}
void VertexBuffer::buffer() {
    glGenBuffers(1, &bufferID);
}
void VertexBuffer::enable() {
    glEnableVertexAttribArray(location);
}
void VertexBuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
}
void VertexBuffer::upload() {
    bind();
    glVertexAttribPointer(0, size, type, normalized, stride, NULL);
    enable();
}
void VertexBuffer::vertexSpec(unsigned int p_size, GLenum p_type, GLboolean p_normalized, GLsizei p_stride) {
    size = p_size;
    type = p_type;
    normalized = p_normalized;
    stride = p_stride;
}