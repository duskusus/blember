#pragma once
#include <GL/glew.h>
#include "shader.hpp"
class Program{
    GLuint id = 0;
    public:
    Program();
    void attachShader(Shader &s);
    void link();   
    void use();
    void getAttribute();
    GLuint getID(); 
};