#include <string>
#include "program.hpp"
class VertexAttrib {
    std::string name;
    GLuint program;
    GLint location;
    GLuint bufferID;
    //same order as parameters to vertexAttribPointer
    unsigned int size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    void *data = nullptr;

    unsigned int count;

    public:
    VertexAttrib(const std::string &n, Program &p);
    void set(void *data, unsigned int bytes);
    void buffer();
    void enable();
    void bind();
    void upload();
    void vertexSpec(unsigned int p_size, GLenum type, GLboolean normalized, GLsizei stride);
};