#pragma once
#include <string>
#include <GL/glew.h>
class Shader {
    std::string source;
    GLuint id;
public:
    Shader(std::string filepath, GLenum shaderType);
    GLuint getID();

};