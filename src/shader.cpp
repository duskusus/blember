#include "shader.hpp"
#include <fstream>
#include <iostream>
Shader::Shader(std::string filepath, GLenum shaderType) {
  printf("Opening shader");
  std::ifstream file(filepath);
  std::string content((std::istreambuf_iterator<char>(file)),
                      (std::istreambuf_iterator<char>()));
  source = content;
  id = glCreateShader(shaderType);
  const char *sourceCStr = source.c_str();
  glShaderSource(id, 1, (const GLchar *const *)&sourceCStr, NULL);
  glCompileShader(id);
  GLint compileSuccess = GL_FALSE;
  glGetShaderiv(id, GL_COMPILE_STATUS, &compileSuccess);
  if (compileSuccess == GL_FALSE) {
    GLint logSize = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logSize);
    char *log = (char *)malloc(logSize + 1);
    glGetShaderInfoLog(id, logSize, nullptr, log);
    std::cout << "Could not compile shader\n";
    printf("length: %d\n%s\n",logSize, log);
    std::cout << source <<  std::endl;
    free(log);
    glDeleteShader(id);
    return;
  }
}
GLuint Shader::getID() {
    return id;
}