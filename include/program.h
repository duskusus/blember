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
    void getAttribute();
    GLuint getID(); 
};