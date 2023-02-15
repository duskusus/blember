#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <iostream>
#include <string>
#include "vertexbuffer.h"
#include <unordered_map>
#include <glm/glm.hpp>

#include "uniform.h"
#include "newchunk.h"
class Context {
    public:
    SDL_Window *window = nullptr;
    bool flying = true;
    SDL_GLContext context = nullptr;
    Uint64 frames = 0;
    const unsigned short height;
    const unsigned short width;
    float sensitivity = 2.5;
    unsigned int keycount;
    bool showFrameInfo = true;
    bool captureMouse = true;
    SDL_Renderer *renderer = nullptr;
    glm::vec2 mouse = glm::vec2(0);
    SDL_Keycode keysToPoll[9] = {
        SDLK_w,
        SDLK_a,
        SDLK_s,
        SDLK_d,
        SDLK_SPACE,
        SDLK_LCTRL,
        SDLK_ESCAPE,
        SDLK_t,
        SDLK_r
    };
    std::unordered_map<SDL_Keycode, bool> *keys;
    Context(std::string name, uint16_t width, uint16_t height);
    void swap();
    int poll();
    void fly_control_view(glm::mat4 &view, glm::vec4 &position, const float deltaTime);
    void walk_control_view(glm::mat4 &view, glm::vec4 &position, const float deltaTime, NewChunk &world);
};