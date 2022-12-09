#pragma once
#include <GL/glew.h>
#include "shader.h"
class Program{
    GLuint id = 0;
    public:
    Program();
    void attachShader(Shader &s);
    void link();   
    void use() const;
    int32_t getAttribute(const char *name);
    GLuint getID() const; 
};