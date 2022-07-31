#include "program.hpp"
#include <GL/glew.h>
#include <SDL2/SDL_opengl_glext.h>
#include <cstddef>
Program::Program() {
  id = glCreateProgram();
  printf("program id: %d\n", id);
}
void Program::attachShader(Shader &s) { glAttachShader(id, s.getID()); }
void Program::link() {
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
  }
}
void Program::use() { glUseProgram(id); }
GLuint Program::getID() { return id; }