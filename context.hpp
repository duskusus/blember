#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <iostream>
#include <string>
#include "vertexattrib.hpp"
class Context {
    SDL_Window *window = nullptr;
    SDL_GLContext context = nullptr;
    const unsigned short height;
    const unsigned short width;
    public:
    Context(std::string name, uint16_t width, uint16_t height);
    void swap();
    int poll();
};