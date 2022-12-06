#include "program.h"

#include <GL/glew.h>
#include <SDL2/SDL_opengl_glext.h>

#include <cstddef>
Program::Program()
{
    id = glCreateProgram();
    printf("program id: %d\n", id);
}
void Program::attachShader(Shader &s) { glAttachShader(id, s.getID()); }
void Program::link()
{
    glLinkProgram(id);
    GLint linkSuccess = GL_FALSE;
    glGetProgramiv(id, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);
        char *log = (char *)malloc(maxLength + 1);
        glGetProgramInfoLog(id, maxLength, nullptr, log);
        printf("Program linking failed\n%s\n", log);
        free(log);
        glDeleteProgram(id);
        return;
    }
    printf("Program link successful\n");
}
void Program::use() const { 
  glUseProgram(id); }
int32_t Program::getAttribute(const char *name) const
{
    const int attrib = glGetAttribLocation(id, name);
    if(attrib < 0) {
        printf("Could not get attribute %s\n", name);
    }
    return attrib;
}
GLuint Program::getID() { return id; }