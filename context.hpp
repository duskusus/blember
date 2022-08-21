#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <iostream>
#include <string>
#include "vertexbuffer.hpp"
#include <unordered_map>
#include <glm/glm.hpp>
class Context {
    public:
    SDL_Window *window = nullptr;
    SDL_GLContext context = nullptr;
    const unsigned short height;
    const unsigned short width;
    float sensitivity = 0.01;
    unsigned int keycount;
    bool showFrameInfo = true;
    bool captureMouse = true;
    glm::vec2 mouse = glm::vec2(0);
    SDL_Keycode keysToPoll[7] = {
        SDLK_w,
        SDLK_a,
        SDLK_s,
        SDLK_d,
        SDLK_SPACE,
        SDLK_LCTRL,
        SDLK_ESCAPE
    };
    std::unordered_map<SDL_Keycode, bool> *keys;
    Context(std::string name, uint16_t width, uint16_t height);
    void swap();
    int poll();
};